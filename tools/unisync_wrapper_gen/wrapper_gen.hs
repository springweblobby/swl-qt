{-# LANGUAGE OverloadedStrings #-}

import System.Environment
import Data.Monoid ((<>))
import Data.Char
import Data.Either
import Data.List (intersperse)
import qualified Data.ByteString.Char8 as B
import Data.ByteString (ByteString)
import Text.Parsec
import Text.Parsec.ByteString
import Control.Applicative ((<$>), (<*>))


marshallIn :: CType -> ByteString -> ByteString
marshallIn QString name = name <> ".toStdString().c_str()"
marshallIn _ name = name

marshallOut :: CType -> ByteString -> ByteString
marshallOut QString name = "QString(" <> name <> ")"
marshallOut _ name = name

marshallAsString :: CType -> ByteString -> ByteString
marshallAsString CBool name = "(" <> name <> " ? \"true\" : \"false\")"
marshallAsString CString name = name
marshallAsString CVoid name = "\"\""
marshallAsString _ name = "std::to_string(" <> name <> ")"

getExternalRep :: CType -> CType
getExternalRep CString = QString
-- Unitsync returns checksums as unsigned integers but the server actually
-- uses signed ints (yay consistency), thus this line.
getExternalRep CUint = CInt
getExternalRep t = t

putTemplate :: [CFunc] -> ByteString -> [ByteString]

-- Async unitsync stuff.

-- Declarations of public methods for UnitsyncHandlerAsync.
putTemplate funcs "header_public_methods_async" = flip map (onlyKnown funcs) $ \(CFunc ret name args) ->
    "void " <> toLowerCase name <> "(" <> commaList ("QString" : map (\(Arg _ t) ->
        showCType (getExternalRep t)) args) <> ");"

-- Definitions of the public methods (async).
putTemplate funcs "public_methods_definitions_async" = concat $ flip map (onlyKnown funcs) $ \(CFunc ret name args) ->
 let callArgs = flip map args $ \(Arg n t) -> marshallIn (getExternalRep t) n
  in ["void UnitsyncHandlerAsync::" <> toLowerCase name <> "(" <>
        commaList ("QString __id" : map (\(Arg n t) -> showCType (getExternalRep t) <> " " <> n) args) <> ") {",
     "    if (fptr_" <> name <> " == NULL) {",
     "        logger.error(\"Bad function pointer: " <> name <> "\");",
     "        throw bad_fptr(\"" <> name <> "\");",
     "    }",
     "    boost::unique_lock<boost::mutex> lock(queueMutex);",
     "    queue.push([=](){",
     "        logger.debug(\"call " <> name <> "(\", " <> commaList (intersperse "\", \"" callArgs) <>
                (if null callArgs then "" else ", ") <> "\")\");",
     "        " <> case ret of
                    (CVoid) -> ""
                    typ -> showCType typ <> " res = "
                <> "fptr_" <> name <> "(" <> commaList callArgs <> ")" <> ";",
     "        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), \"" <> showCType ret <> "\", " <>
                marshallAsString ret "res" <> "));",
     "    });",
     "    queueCond.notify_all();",
     "}"]



-- This goes into the move ctor of UnitsyncHandler.
putTemplate funcs "move_ctor_assignment" = map (\(CFunc _ name _) ->
    "fptr_" <> name <> " = h.fptr_" <> name <> ";") funcs

-- Initialize function pointers on Unix with dlsym().
putTemplate funcs "fptr_initialization_unix" = map (\(CFunc _ name _) ->
    "fptr_" <> name <> " = (fptr_type_" <> name <> ")dlsym(handle, \"" <> name <> "\");") funcs

-- Initialize function pointers on Windows with GetProcAddress().
putTemplate funcs "fptr_initialization_windows" = map (\(CFunc _ name _) ->
    "fptr_" <> name <> " = (fptr_type_" <> name <> ")GetProcAddress((HMODULE)handle, \"" <> name <> "\");") funcs

-- Declarations of function pointer types and pointers themselves.
putTemplate funcs "header_properties" = concat $ flip map funcs $ \(CFunc ret name args) ->
    ["typedef " <> showCType ret <> " (*fptr_type_" <> name <> ")(" <> commaList (map (\(Arg _ t) -> showCType t) args) <> ");",
     "fptr_type_" <> name <> " fptr_" <> name <> ";"]

-- Declarations of public methods for UnitsyncHandler.
putTemplate funcs "header_public_methods" = flip map (onlyKnown funcs) $ \(CFunc ret name args) ->
    showCType (getExternalRep ret) <> " " <> toLowerCase name <> "(" <> commaList (map (\(Arg _ t) ->
        showCType (getExternalRep t)) args) <> ");"

-- Definitions of the public methods.
putTemplate funcs "public_methods_definitions" = concat $ flip map (onlyKnown funcs) $ \(CFunc ret name args) ->
 let callArgs = flip map args $ \(Arg n t) -> marshallIn (getExternalRep t) n
  in [showCType (getExternalRep ret) <> " UnitsyncHandler::" <> toLowerCase name <> "(" <>
        commaList (map (\(Arg n t) -> showCType (getExternalRep t) <> " " <> n) args) <> ") {",
     "    if (fptr_" <> name <> " == NULL) {",
     "        logger.error(\"Bad function pointer: " <> name <> "\");",
     "        throw bad_fptr(\"" <> name <> "\");",
     "    }",
     "    logger.debug(\"call " <> name <> "(\", " <> commaList (intersperse "\", \"" callArgs) <>
            (if null callArgs then "" else ", ") <> "\")\");",
     "    return " <> marshallOut (getExternalRep ret) ("fptr_" <> name <> "(" <> commaList callArgs <> ")") <> ";",
     "}"]

putTemplate _ name = error $ "Unknown template target " ++ B.unpack name

processTemplate :: [CFunc] -> String -> IO ()
processTemplate funcs inFile = do
    let outFile = reverse $ drop 9 $ reverse inFile -- strip ".template"
    putStrLn $ inFile ++ " -> " ++ outFile
    B.readFile inFile >>= B.writeFile outFile . B.unlines . scan . B.lines
    where scan [] = []
          scan (l:ls) = case parse p "" l of
                            (Left _) -> l : scan ls
                            (Right (name, indent)) -> (map (indent<>) $ putTemplate funcs name) ++ scan ls
          p = do
            indent <- fmap B.pack $ many space
            string "${"
            name <- fmap B.pack $ many1 $ noneOf "}"
            string "}"
            return (name, indent)

onlyKnown :: [CFunc] -> [CFunc]
onlyKnown = filter flt
    where flt (CFunc (CUnknown _) _ _) = False
          flt (CFunc _ _ args) = not $ any isU $ map (\(Arg _ t) -> t) args
          isU (CUnknown _) = True
          isU _ = False

toLowerCase :: ByteString -> ByteString
toLowerCase "" = ""
toLowerCase str = let (Just (c, rest)) = B.uncons str in B.cons (toLower c) rest

commaList :: [ByteString] -> ByteString
commaList [] = ""
commaList xs = foldr1 (\a b -> a <> ", " <> b) xs

-- Parse the API header for exported functions.

data CFunc = CFunc CType ByteString [Arg] deriving(Show)
data Arg = Arg ByteString CType deriving(Show)
data CType = CVoid | CUint | CInt | CBool | CFloat | CString | QString | CUnknown ByteString deriving(Show)

showCType :: CType -> ByteString
showCType CVoid = "void"
showCType CUint = "unsigned int"
showCType CInt = "int"
showCType CBool = "bool"
showCType CFloat = "float"
showCType CString = "const char*"
showCType QString = "QString"
showCType (CUnknown s) = s

parseCType :: Parser CType
parseCType = tryType ["const", "char", "*"] CString <|> tryType ["unsigned", "int"] CUint <|>
    tryType ["unsigned", "short", "*"] (CUnknown "unsigned short*") <|> tryType ["int", "*"] (CUnknown "int*") <|>
    tryType ["int"] CInt <|> tryType ["const", "int"] CInt <|> tryType ["float"] CFloat <|>
    tryType ["const float"] CFloat <|> tryType ["bool"] CBool <|> tryType ["char", "*"] (CUnknown "char*") <|>
    tryType ["unsigned", "char", "*"] (CUnknown "unsigned char*") <|> tryType ["void"] CVoid <|>
    tryType ["MapInfo", "*"] (CUnknown "MapInfo*") <|>
    parserFail "unknown type (add to parseCType)"
    where f [] ret = return ret
          f (x:xs) ret = string x >> spaces >> f xs ret
          tryType ws ret = try (f ws ret)

argument :: Parser Arg
argument = (flip Arg) <$> parseCType <*> fmap B.pack (spaces >> many1 (try alphaNum <|> oneOf "-_"))

function :: Parser CFunc
function = do
    string "EXPORT(" >> spaces
    ret <- parseCType
    spaces >> char ')' >> spaces
    name <- fmap B.pack $ many1 $ try alphaNum <|> oneOf "-_"
    spaces >> char '('
    args <- sepBy argument $ spaces >> char ',' >> spaces
    spaces >> char ')' >> spaces >> char ';' >> skipMany (char ' ') -- "spaces" parses newlines too!
    return $ CFunc ret name args

functions :: Parser [Either ByteString CFunc]
functions = do
    r <- sepEndBy1 (fmap  Right (try function) <|> fmap Left (try comment) <?> "function defenition or comment") $ skipMany newline
    eof
    return r
    where comment = try (spaces >> string "/*" >> manyTill anyChar (try (string "*/")) >> return "/**/") <|>
            ((try (spaces >> string "//") <|> try (spaces >> string "#")) >> many (noneOf ['\n']) >> return "//")

main = do
    args <- getArgs
    if null args then
        putStrLn "Usage: ./wrapper_gen unitsync_api.h unitsynchandler.h.template unitsynchandler.cpp.template"
    else do
        res <- parseFromFile functions $ head args
        case res of
            (Left e) -> print e
            (Right fs') -> do
                let fs = rights fs'
                putStrLn $ "Parsing " ++ (head args) ++ "..."
                mapM_ print fs
                --mapM_ print fs'
                putStr $ show $ length fs
                putStrLn " functions, processing templates...\n"
                mapM_ (processTemplate fs) $ tail args
