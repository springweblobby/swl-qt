#include "unitsynchandler_t.h"
#include <cstdio> // good ol' snprintf
#include <boost/thread/locks.hpp>
#if defined Q_OS_LINUX || defined Q_OS_MAC
    #include <dlfcn.h>
#elif defined Q_OS_WIN32
	#include <windows.h>
    #ifndef LOAD_LIBRARY_SEARCH_DEFAULT_DIRS
        #define LOAD_LIBRARY_SEARCH_DEFAULT_DIRS 0x1000
    #endif
    #ifndef LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR
        #define LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR 0x100
    #endif
#else
    #error "Unknown target OS."
#endif

boost::mutex UnitsyncHandlerAsync::executionMutex;

bool UnitsyncHandlerAsync::startThread() {
    if (ready) {
        workThread = boost::thread([=](){
            std::function<void()> func;
            while (ready) {{
                    boost::unique_lock<boost::mutex> lock(queueMutex);
                    queueCond.wait(lock, [=](){ return !(ready && queue.empty()); });
                    if (!ready) break;
                    func = queue.front();
                    queue.pop();
                }{
                    boost::lock_guard<boost::mutex> lock(executionMutex);
                    func();
                }
            }
        });
        return true;
    } else
        return false;
}

UnitsyncHandlerAsync::UnitsyncHandlerAsync(QObject* parent, Logger& logger, boost::filesystem::path path) :
        QObject(parent), logger(logger), ready(false), handle(NULL) {
    logger.info("Loading unitsync at ", path);
    #if defined Q_OS_LINUX || defined Q_OS_MAC
        handle = dlopen(path.c_str(), RTLD_LAZY | RTLD_LOCAL);
        if(handle == NULL) {
            logger.warning("Could not load unitsync at ", path, ": ", dlerror());
            return;
        }

        fptr_GetNextError = (fptr_type_GetNextError)dlsym(handle, "GetNextError");
        fptr_GetSpringVersion = (fptr_type_GetSpringVersion)dlsym(handle, "GetSpringVersion");
        fptr_GetSpringVersionPatchset = (fptr_type_GetSpringVersionPatchset)dlsym(handle, "GetSpringVersionPatchset");
        fptr_IsSpringReleaseVersion = (fptr_type_IsSpringReleaseVersion)dlsym(handle, "IsSpringReleaseVersion");
        fptr_Init = (fptr_type_Init)dlsym(handle, "Init");
        fptr_UnInit = (fptr_type_UnInit)dlsym(handle, "UnInit");
        fptr_GetWritableDataDirectory = (fptr_type_GetWritableDataDirectory)dlsym(handle, "GetWritableDataDirectory");
        fptr_GetDataDirectoryCount = (fptr_type_GetDataDirectoryCount)dlsym(handle, "GetDataDirectoryCount");
        fptr_GetDataDirectory = (fptr_type_GetDataDirectory)dlsym(handle, "GetDataDirectory");
        fptr_ProcessUnits = (fptr_type_ProcessUnits)dlsym(handle, "ProcessUnits");
        fptr_GetUnitCount = (fptr_type_GetUnitCount)dlsym(handle, "GetUnitCount");
        fptr_GetUnitName = (fptr_type_GetUnitName)dlsym(handle, "GetUnitName");
        fptr_GetFullUnitName = (fptr_type_GetFullUnitName)dlsym(handle, "GetFullUnitName");
        fptr_AddArchive = (fptr_type_AddArchive)dlsym(handle, "AddArchive");
        fptr_AddAllArchives = (fptr_type_AddAllArchives)dlsym(handle, "AddAllArchives");
        fptr_RemoveAllArchives = (fptr_type_RemoveAllArchives)dlsym(handle, "RemoveAllArchives");
        fptr_GetArchiveChecksum = (fptr_type_GetArchiveChecksum)dlsym(handle, "GetArchiveChecksum");
        fptr_GetArchivePath = (fptr_type_GetArchivePath)dlsym(handle, "GetArchivePath");
        fptr_GetMapCount = (fptr_type_GetMapCount)dlsym(handle, "GetMapCount");
        fptr_GetMapName = (fptr_type_GetMapName)dlsym(handle, "GetMapName");
        fptr_GetMapFileName = (fptr_type_GetMapFileName)dlsym(handle, "GetMapFileName");
        fptr_GetMapDescription = (fptr_type_GetMapDescription)dlsym(handle, "GetMapDescription");
        fptr_GetMapAuthor = (fptr_type_GetMapAuthor)dlsym(handle, "GetMapAuthor");
        fptr_GetMapWidth = (fptr_type_GetMapWidth)dlsym(handle, "GetMapWidth");
        fptr_GetMapHeight = (fptr_type_GetMapHeight)dlsym(handle, "GetMapHeight");
        fptr_GetMapTidalStrength = (fptr_type_GetMapTidalStrength)dlsym(handle, "GetMapTidalStrength");
        fptr_GetMapWindMin = (fptr_type_GetMapWindMin)dlsym(handle, "GetMapWindMin");
        fptr_GetMapWindMax = (fptr_type_GetMapWindMax)dlsym(handle, "GetMapWindMax");
        fptr_GetMapGravity = (fptr_type_GetMapGravity)dlsym(handle, "GetMapGravity");
        fptr_GetMapResourceCount = (fptr_type_GetMapResourceCount)dlsym(handle, "GetMapResourceCount");
        fptr_GetMapResourceName = (fptr_type_GetMapResourceName)dlsym(handle, "GetMapResourceName");
        fptr_GetMapResourceMax = (fptr_type_GetMapResourceMax)dlsym(handle, "GetMapResourceMax");
        fptr_GetMapResourceExtractorRadius = (fptr_type_GetMapResourceExtractorRadius)dlsym(handle, "GetMapResourceExtractorRadius");
        fptr_GetMapPosCount = (fptr_type_GetMapPosCount)dlsym(handle, "GetMapPosCount");
        fptr_GetMapPosX = (fptr_type_GetMapPosX)dlsym(handle, "GetMapPosX");
        fptr_GetMapPosZ = (fptr_type_GetMapPosZ)dlsym(handle, "GetMapPosZ");
        fptr_GetMapMinHeight = (fptr_type_GetMapMinHeight)dlsym(handle, "GetMapMinHeight");
        fptr_GetMapMaxHeight = (fptr_type_GetMapMaxHeight)dlsym(handle, "GetMapMaxHeight");
        fptr_GetMapArchiveCount = (fptr_type_GetMapArchiveCount)dlsym(handle, "GetMapArchiveCount");
        fptr_GetMapArchiveName = (fptr_type_GetMapArchiveName)dlsym(handle, "GetMapArchiveName");
        fptr_GetMapChecksum = (fptr_type_GetMapChecksum)dlsym(handle, "GetMapChecksum");
        fptr_GetMapChecksumFromName = (fptr_type_GetMapChecksumFromName)dlsym(handle, "GetMapChecksumFromName");
        fptr_GetMinimap = (fptr_type_GetMinimap)dlsym(handle, "GetMinimap");
        fptr_GetInfoMapSize = (fptr_type_GetInfoMapSize)dlsym(handle, "GetInfoMapSize");
        fptr_GetInfoMap = (fptr_type_GetInfoMap)dlsym(handle, "GetInfoMap");
        fptr_GetSkirmishAICount = (fptr_type_GetSkirmishAICount)dlsym(handle, "GetSkirmishAICount");
        fptr_GetSkirmishAIInfoCount = (fptr_type_GetSkirmishAIInfoCount)dlsym(handle, "GetSkirmishAIInfoCount");
        fptr_GetInfoKey = (fptr_type_GetInfoKey)dlsym(handle, "GetInfoKey");
        fptr_GetInfoType = (fptr_type_GetInfoType)dlsym(handle, "GetInfoType");
        fptr_GetInfoValueString = (fptr_type_GetInfoValueString)dlsym(handle, "GetInfoValueString");
        fptr_GetInfoValueInteger = (fptr_type_GetInfoValueInteger)dlsym(handle, "GetInfoValueInteger");
        fptr_GetInfoValueFloat = (fptr_type_GetInfoValueFloat)dlsym(handle, "GetInfoValueFloat");
        fptr_GetInfoValueBool = (fptr_type_GetInfoValueBool)dlsym(handle, "GetInfoValueBool");
        fptr_GetInfoDescription = (fptr_type_GetInfoDescription)dlsym(handle, "GetInfoDescription");
        fptr_GetSkirmishAIOptionCount = (fptr_type_GetSkirmishAIOptionCount)dlsym(handle, "GetSkirmishAIOptionCount");
        fptr_GetPrimaryModCount = (fptr_type_GetPrimaryModCount)dlsym(handle, "GetPrimaryModCount");
        fptr_GetPrimaryModInfoCount = (fptr_type_GetPrimaryModInfoCount)dlsym(handle, "GetPrimaryModInfoCount");
        fptr_GetPrimaryModArchive = (fptr_type_GetPrimaryModArchive)dlsym(handle, "GetPrimaryModArchive");
        fptr_GetPrimaryModArchiveCount = (fptr_type_GetPrimaryModArchiveCount)dlsym(handle, "GetPrimaryModArchiveCount");
        fptr_GetPrimaryModArchiveList = (fptr_type_GetPrimaryModArchiveList)dlsym(handle, "GetPrimaryModArchiveList");
        fptr_GetPrimaryModIndex = (fptr_type_GetPrimaryModIndex)dlsym(handle, "GetPrimaryModIndex");
        fptr_GetPrimaryModChecksum = (fptr_type_GetPrimaryModChecksum)dlsym(handle, "GetPrimaryModChecksum");
        fptr_GetPrimaryModChecksumFromName = (fptr_type_GetPrimaryModChecksumFromName)dlsym(handle, "GetPrimaryModChecksumFromName");
        fptr_GetSideCount = (fptr_type_GetSideCount)dlsym(handle, "GetSideCount");
        fptr_GetSideName = (fptr_type_GetSideName)dlsym(handle, "GetSideName");
        fptr_GetSideStartUnit = (fptr_type_GetSideStartUnit)dlsym(handle, "GetSideStartUnit");
        fptr_GetMapOptionCount = (fptr_type_GetMapOptionCount)dlsym(handle, "GetMapOptionCount");
        fptr_GetModOptionCount = (fptr_type_GetModOptionCount)dlsym(handle, "GetModOptionCount");
        fptr_GetCustomOptionCount = (fptr_type_GetCustomOptionCount)dlsym(handle, "GetCustomOptionCount");
        fptr_GetOptionKey = (fptr_type_GetOptionKey)dlsym(handle, "GetOptionKey");
        fptr_GetOptionScope = (fptr_type_GetOptionScope)dlsym(handle, "GetOptionScope");
        fptr_GetOptionName = (fptr_type_GetOptionName)dlsym(handle, "GetOptionName");
        fptr_GetOptionSection = (fptr_type_GetOptionSection)dlsym(handle, "GetOptionSection");
        fptr_GetOptionStyle = (fptr_type_GetOptionStyle)dlsym(handle, "GetOptionStyle");
        fptr_GetOptionDesc = (fptr_type_GetOptionDesc)dlsym(handle, "GetOptionDesc");
        fptr_GetOptionType = (fptr_type_GetOptionType)dlsym(handle, "GetOptionType");
        fptr_GetOptionBoolDef = (fptr_type_GetOptionBoolDef)dlsym(handle, "GetOptionBoolDef");
        fptr_GetOptionNumberDef = (fptr_type_GetOptionNumberDef)dlsym(handle, "GetOptionNumberDef");
        fptr_GetOptionNumberMin = (fptr_type_GetOptionNumberMin)dlsym(handle, "GetOptionNumberMin");
        fptr_GetOptionNumberMax = (fptr_type_GetOptionNumberMax)dlsym(handle, "GetOptionNumberMax");
        fptr_GetOptionNumberStep = (fptr_type_GetOptionNumberStep)dlsym(handle, "GetOptionNumberStep");
        fptr_GetOptionStringDef = (fptr_type_GetOptionStringDef)dlsym(handle, "GetOptionStringDef");
        fptr_GetOptionStringMaxLen = (fptr_type_GetOptionStringMaxLen)dlsym(handle, "GetOptionStringMaxLen");
        fptr_GetOptionListCount = (fptr_type_GetOptionListCount)dlsym(handle, "GetOptionListCount");
        fptr_GetOptionListDef = (fptr_type_GetOptionListDef)dlsym(handle, "GetOptionListDef");
        fptr_GetOptionListItemKey = (fptr_type_GetOptionListItemKey)dlsym(handle, "GetOptionListItemKey");
        fptr_GetOptionListItemName = (fptr_type_GetOptionListItemName)dlsym(handle, "GetOptionListItemName");
        fptr_GetOptionListItemDesc = (fptr_type_GetOptionListItemDesc)dlsym(handle, "GetOptionListItemDesc");
        fptr_GetModValidMapCount = (fptr_type_GetModValidMapCount)dlsym(handle, "GetModValidMapCount");
        fptr_GetModValidMap = (fptr_type_GetModValidMap)dlsym(handle, "GetModValidMap");
        fptr_OpenFileVFS = (fptr_type_OpenFileVFS)dlsym(handle, "OpenFileVFS");
        fptr_CloseFileVFS = (fptr_type_CloseFileVFS)dlsym(handle, "CloseFileVFS");
        fptr_ReadFileVFS = (fptr_type_ReadFileVFS)dlsym(handle, "ReadFileVFS");
        fptr_FileSizeVFS = (fptr_type_FileSizeVFS)dlsym(handle, "FileSizeVFS");
        fptr_InitFindVFS = (fptr_type_InitFindVFS)dlsym(handle, "InitFindVFS");
        fptr_InitDirListVFS = (fptr_type_InitDirListVFS)dlsym(handle, "InitDirListVFS");
        fptr_InitSubDirsVFS = (fptr_type_InitSubDirsVFS)dlsym(handle, "InitSubDirsVFS");
        fptr_FindFilesVFS = (fptr_type_FindFilesVFS)dlsym(handle, "FindFilesVFS");
        fptr_OpenArchive = (fptr_type_OpenArchive)dlsym(handle, "OpenArchive");
        fptr_CloseArchive = (fptr_type_CloseArchive)dlsym(handle, "CloseArchive");
        fptr_FindFilesArchive = (fptr_type_FindFilesArchive)dlsym(handle, "FindFilesArchive");
        fptr_OpenArchiveFile = (fptr_type_OpenArchiveFile)dlsym(handle, "OpenArchiveFile");
        fptr_ReadArchiveFile = (fptr_type_ReadArchiveFile)dlsym(handle, "ReadArchiveFile");
        fptr_CloseArchiveFile = (fptr_type_CloseArchiveFile)dlsym(handle, "CloseArchiveFile");
        fptr_SizeArchiveFile = (fptr_type_SizeArchiveFile)dlsym(handle, "SizeArchiveFile");
        fptr_SetSpringConfigFile = (fptr_type_SetSpringConfigFile)dlsym(handle, "SetSpringConfigFile");
        fptr_GetSpringConfigFile = (fptr_type_GetSpringConfigFile)dlsym(handle, "GetSpringConfigFile");
        fptr_GetSpringConfigString = (fptr_type_GetSpringConfigString)dlsym(handle, "GetSpringConfigString");
        fptr_GetSpringConfigInt = (fptr_type_GetSpringConfigInt)dlsym(handle, "GetSpringConfigInt");
        fptr_GetSpringConfigFloat = (fptr_type_GetSpringConfigFloat)dlsym(handle, "GetSpringConfigFloat");
        fptr_SetSpringConfigString = (fptr_type_SetSpringConfigString)dlsym(handle, "SetSpringConfigString");
        fptr_SetSpringConfigInt = (fptr_type_SetSpringConfigInt)dlsym(handle, "SetSpringConfigInt");
        fptr_SetSpringConfigFloat = (fptr_type_SetSpringConfigFloat)dlsym(handle, "SetSpringConfigFloat");
        fptr_DeleteSpringConfigKey = (fptr_type_DeleteSpringConfigKey)dlsym(handle, "DeleteSpringConfigKey");
        fptr_lpClose = (fptr_type_lpClose)dlsym(handle, "lpClose");
        fptr_lpOpenFile = (fptr_type_lpOpenFile)dlsym(handle, "lpOpenFile");
        fptr_lpOpenSource = (fptr_type_lpOpenSource)dlsym(handle, "lpOpenSource");
        fptr_lpExecute = (fptr_type_lpExecute)dlsym(handle, "lpExecute");
        fptr_lpErrorLog = (fptr_type_lpErrorLog)dlsym(handle, "lpErrorLog");
        fptr_lpAddTableInt = (fptr_type_lpAddTableInt)dlsym(handle, "lpAddTableInt");
        fptr_lpAddTableStr = (fptr_type_lpAddTableStr)dlsym(handle, "lpAddTableStr");
        fptr_lpEndTable = (fptr_type_lpEndTable)dlsym(handle, "lpEndTable");
        fptr_lpAddIntKeyIntVal = (fptr_type_lpAddIntKeyIntVal)dlsym(handle, "lpAddIntKeyIntVal");
        fptr_lpAddStrKeyIntVal = (fptr_type_lpAddStrKeyIntVal)dlsym(handle, "lpAddStrKeyIntVal");
        fptr_lpAddIntKeyBoolVal = (fptr_type_lpAddIntKeyBoolVal)dlsym(handle, "lpAddIntKeyBoolVal");
        fptr_lpAddStrKeyBoolVal = (fptr_type_lpAddStrKeyBoolVal)dlsym(handle, "lpAddStrKeyBoolVal");
        fptr_lpAddIntKeyFloatVal = (fptr_type_lpAddIntKeyFloatVal)dlsym(handle, "lpAddIntKeyFloatVal");
        fptr_lpAddStrKeyFloatVal = (fptr_type_lpAddStrKeyFloatVal)dlsym(handle, "lpAddStrKeyFloatVal");
        fptr_lpAddIntKeyStrVal = (fptr_type_lpAddIntKeyStrVal)dlsym(handle, "lpAddIntKeyStrVal");
        fptr_lpAddStrKeyStrVal = (fptr_type_lpAddStrKeyStrVal)dlsym(handle, "lpAddStrKeyStrVal");
        fptr_lpRootTable = (fptr_type_lpRootTable)dlsym(handle, "lpRootTable");
        fptr_lpRootTableExpr = (fptr_type_lpRootTableExpr)dlsym(handle, "lpRootTableExpr");
        fptr_lpSubTableInt = (fptr_type_lpSubTableInt)dlsym(handle, "lpSubTableInt");
        fptr_lpSubTableStr = (fptr_type_lpSubTableStr)dlsym(handle, "lpSubTableStr");
        fptr_lpSubTableExpr = (fptr_type_lpSubTableExpr)dlsym(handle, "lpSubTableExpr");
        fptr_lpPopTable = (fptr_type_lpPopTable)dlsym(handle, "lpPopTable");
        fptr_lpGetKeyExistsInt = (fptr_type_lpGetKeyExistsInt)dlsym(handle, "lpGetKeyExistsInt");
        fptr_lpGetKeyExistsStr = (fptr_type_lpGetKeyExistsStr)dlsym(handle, "lpGetKeyExistsStr");
        fptr_lpGetIntKeyType = (fptr_type_lpGetIntKeyType)dlsym(handle, "lpGetIntKeyType");
        fptr_lpGetStrKeyType = (fptr_type_lpGetStrKeyType)dlsym(handle, "lpGetStrKeyType");
        fptr_lpGetIntKeyListCount = (fptr_type_lpGetIntKeyListCount)dlsym(handle, "lpGetIntKeyListCount");
        fptr_lpGetIntKeyListEntry = (fptr_type_lpGetIntKeyListEntry)dlsym(handle, "lpGetIntKeyListEntry");
        fptr_lpGetStrKeyListCount = (fptr_type_lpGetStrKeyListCount)dlsym(handle, "lpGetStrKeyListCount");
        fptr_lpGetStrKeyListEntry = (fptr_type_lpGetStrKeyListEntry)dlsym(handle, "lpGetStrKeyListEntry");
        fptr_lpGetIntKeyIntVal = (fptr_type_lpGetIntKeyIntVal)dlsym(handle, "lpGetIntKeyIntVal");
        fptr_lpGetStrKeyIntVal = (fptr_type_lpGetStrKeyIntVal)dlsym(handle, "lpGetStrKeyIntVal");
        fptr_lpGetIntKeyBoolVal = (fptr_type_lpGetIntKeyBoolVal)dlsym(handle, "lpGetIntKeyBoolVal");
        fptr_lpGetStrKeyBoolVal = (fptr_type_lpGetStrKeyBoolVal)dlsym(handle, "lpGetStrKeyBoolVal");
        fptr_lpGetIntKeyFloatVal = (fptr_type_lpGetIntKeyFloatVal)dlsym(handle, "lpGetIntKeyFloatVal");
        fptr_lpGetStrKeyFloatVal = (fptr_type_lpGetStrKeyFloatVal)dlsym(handle, "lpGetStrKeyFloatVal");
        fptr_lpGetIntKeyStrVal = (fptr_type_lpGetIntKeyStrVal)dlsym(handle, "lpGetIntKeyStrVal");
        fptr_lpGetStrKeyStrVal = (fptr_type_lpGetStrKeyStrVal)dlsym(handle, "lpGetStrKeyStrVal");
        fptr_ProcessUnitsNoChecksum = (fptr_type_ProcessUnitsNoChecksum)dlsym(handle, "ProcessUnitsNoChecksum");
        fptr_GetMapInfoEx = (fptr_type_GetMapInfoEx)dlsym(handle, "GetMapInfoEx");
        fptr_GetMapInfo = (fptr_type_GetMapInfo)dlsym(handle, "GetMapInfo");
        fptr_GetInfoValue = (fptr_type_GetInfoValue)dlsym(handle, "GetInfoValue");
        fptr_GetPrimaryModName = (fptr_type_GetPrimaryModName)dlsym(handle, "GetPrimaryModName");
        fptr_GetPrimaryModShortName = (fptr_type_GetPrimaryModShortName)dlsym(handle, "GetPrimaryModShortName");
        fptr_GetPrimaryModVersion = (fptr_type_GetPrimaryModVersion)dlsym(handle, "GetPrimaryModVersion");
        fptr_GetPrimaryModMutator = (fptr_type_GetPrimaryModMutator)dlsym(handle, "GetPrimaryModMutator");
        fptr_GetPrimaryModGame = (fptr_type_GetPrimaryModGame)dlsym(handle, "GetPrimaryModGame");
        fptr_GetPrimaryModShortGame = (fptr_type_GetPrimaryModShortGame)dlsym(handle, "GetPrimaryModShortGame");
        fptr_GetPrimaryModDescription = (fptr_type_GetPrimaryModDescription)dlsym(handle, "GetPrimaryModDescription");
        fptr_OpenArchiveType = (fptr_type_OpenArchiveType)dlsym(handle, "OpenArchiveType");

        ready = true;
    #elif defined Q_OS_WIN32
        handle = LoadLibraryEx(path.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
        if(handle == NULL) {
            logger.warning("Could not load unitsync at ", path, ": ", GetLastError());
            return;
        }

        fptr_GetNextError = (fptr_type_GetNextError)GetProcAddress((HMODULE)handle, "GetNextError");
        fptr_GetSpringVersion = (fptr_type_GetSpringVersion)GetProcAddress((HMODULE)handle, "GetSpringVersion");
        fptr_GetSpringVersionPatchset = (fptr_type_GetSpringVersionPatchset)GetProcAddress((HMODULE)handle, "GetSpringVersionPatchset");
        fptr_IsSpringReleaseVersion = (fptr_type_IsSpringReleaseVersion)GetProcAddress((HMODULE)handle, "IsSpringReleaseVersion");
        fptr_Init = (fptr_type_Init)GetProcAddress((HMODULE)handle, "Init");
        fptr_UnInit = (fptr_type_UnInit)GetProcAddress((HMODULE)handle, "UnInit");
        fptr_GetWritableDataDirectory = (fptr_type_GetWritableDataDirectory)GetProcAddress((HMODULE)handle, "GetWritableDataDirectory");
        fptr_GetDataDirectoryCount = (fptr_type_GetDataDirectoryCount)GetProcAddress((HMODULE)handle, "GetDataDirectoryCount");
        fptr_GetDataDirectory = (fptr_type_GetDataDirectory)GetProcAddress((HMODULE)handle, "GetDataDirectory");
        fptr_ProcessUnits = (fptr_type_ProcessUnits)GetProcAddress((HMODULE)handle, "ProcessUnits");
        fptr_GetUnitCount = (fptr_type_GetUnitCount)GetProcAddress((HMODULE)handle, "GetUnitCount");
        fptr_GetUnitName = (fptr_type_GetUnitName)GetProcAddress((HMODULE)handle, "GetUnitName");
        fptr_GetFullUnitName = (fptr_type_GetFullUnitName)GetProcAddress((HMODULE)handle, "GetFullUnitName");
        fptr_AddArchive = (fptr_type_AddArchive)GetProcAddress((HMODULE)handle, "AddArchive");
        fptr_AddAllArchives = (fptr_type_AddAllArchives)GetProcAddress((HMODULE)handle, "AddAllArchives");
        fptr_RemoveAllArchives = (fptr_type_RemoveAllArchives)GetProcAddress((HMODULE)handle, "RemoveAllArchives");
        fptr_GetArchiveChecksum = (fptr_type_GetArchiveChecksum)GetProcAddress((HMODULE)handle, "GetArchiveChecksum");
        fptr_GetArchivePath = (fptr_type_GetArchivePath)GetProcAddress((HMODULE)handle, "GetArchivePath");
        fptr_GetMapCount = (fptr_type_GetMapCount)GetProcAddress((HMODULE)handle, "GetMapCount");
        fptr_GetMapName = (fptr_type_GetMapName)GetProcAddress((HMODULE)handle, "GetMapName");
        fptr_GetMapFileName = (fptr_type_GetMapFileName)GetProcAddress((HMODULE)handle, "GetMapFileName");
        fptr_GetMapDescription = (fptr_type_GetMapDescription)GetProcAddress((HMODULE)handle, "GetMapDescription");
        fptr_GetMapAuthor = (fptr_type_GetMapAuthor)GetProcAddress((HMODULE)handle, "GetMapAuthor");
        fptr_GetMapWidth = (fptr_type_GetMapWidth)GetProcAddress((HMODULE)handle, "GetMapWidth");
        fptr_GetMapHeight = (fptr_type_GetMapHeight)GetProcAddress((HMODULE)handle, "GetMapHeight");
        fptr_GetMapTidalStrength = (fptr_type_GetMapTidalStrength)GetProcAddress((HMODULE)handle, "GetMapTidalStrength");
        fptr_GetMapWindMin = (fptr_type_GetMapWindMin)GetProcAddress((HMODULE)handle, "GetMapWindMin");
        fptr_GetMapWindMax = (fptr_type_GetMapWindMax)GetProcAddress((HMODULE)handle, "GetMapWindMax");
        fptr_GetMapGravity = (fptr_type_GetMapGravity)GetProcAddress((HMODULE)handle, "GetMapGravity");
        fptr_GetMapResourceCount = (fptr_type_GetMapResourceCount)GetProcAddress((HMODULE)handle, "GetMapResourceCount");
        fptr_GetMapResourceName = (fptr_type_GetMapResourceName)GetProcAddress((HMODULE)handle, "GetMapResourceName");
        fptr_GetMapResourceMax = (fptr_type_GetMapResourceMax)GetProcAddress((HMODULE)handle, "GetMapResourceMax");
        fptr_GetMapResourceExtractorRadius = (fptr_type_GetMapResourceExtractorRadius)GetProcAddress((HMODULE)handle, "GetMapResourceExtractorRadius");
        fptr_GetMapPosCount = (fptr_type_GetMapPosCount)GetProcAddress((HMODULE)handle, "GetMapPosCount");
        fptr_GetMapPosX = (fptr_type_GetMapPosX)GetProcAddress((HMODULE)handle, "GetMapPosX");
        fptr_GetMapPosZ = (fptr_type_GetMapPosZ)GetProcAddress((HMODULE)handle, "GetMapPosZ");
        fptr_GetMapMinHeight = (fptr_type_GetMapMinHeight)GetProcAddress((HMODULE)handle, "GetMapMinHeight");
        fptr_GetMapMaxHeight = (fptr_type_GetMapMaxHeight)GetProcAddress((HMODULE)handle, "GetMapMaxHeight");
        fptr_GetMapArchiveCount = (fptr_type_GetMapArchiveCount)GetProcAddress((HMODULE)handle, "GetMapArchiveCount");
        fptr_GetMapArchiveName = (fptr_type_GetMapArchiveName)GetProcAddress((HMODULE)handle, "GetMapArchiveName");
        fptr_GetMapChecksum = (fptr_type_GetMapChecksum)GetProcAddress((HMODULE)handle, "GetMapChecksum");
        fptr_GetMapChecksumFromName = (fptr_type_GetMapChecksumFromName)GetProcAddress((HMODULE)handle, "GetMapChecksumFromName");
        fptr_GetMinimap = (fptr_type_GetMinimap)GetProcAddress((HMODULE)handle, "GetMinimap");
        fptr_GetInfoMapSize = (fptr_type_GetInfoMapSize)GetProcAddress((HMODULE)handle, "GetInfoMapSize");
        fptr_GetInfoMap = (fptr_type_GetInfoMap)GetProcAddress((HMODULE)handle, "GetInfoMap");
        fptr_GetSkirmishAICount = (fptr_type_GetSkirmishAICount)GetProcAddress((HMODULE)handle, "GetSkirmishAICount");
        fptr_GetSkirmishAIInfoCount = (fptr_type_GetSkirmishAIInfoCount)GetProcAddress((HMODULE)handle, "GetSkirmishAIInfoCount");
        fptr_GetInfoKey = (fptr_type_GetInfoKey)GetProcAddress((HMODULE)handle, "GetInfoKey");
        fptr_GetInfoType = (fptr_type_GetInfoType)GetProcAddress((HMODULE)handle, "GetInfoType");
        fptr_GetInfoValueString = (fptr_type_GetInfoValueString)GetProcAddress((HMODULE)handle, "GetInfoValueString");
        fptr_GetInfoValueInteger = (fptr_type_GetInfoValueInteger)GetProcAddress((HMODULE)handle, "GetInfoValueInteger");
        fptr_GetInfoValueFloat = (fptr_type_GetInfoValueFloat)GetProcAddress((HMODULE)handle, "GetInfoValueFloat");
        fptr_GetInfoValueBool = (fptr_type_GetInfoValueBool)GetProcAddress((HMODULE)handle, "GetInfoValueBool");
        fptr_GetInfoDescription = (fptr_type_GetInfoDescription)GetProcAddress((HMODULE)handle, "GetInfoDescription");
        fptr_GetSkirmishAIOptionCount = (fptr_type_GetSkirmishAIOptionCount)GetProcAddress((HMODULE)handle, "GetSkirmishAIOptionCount");
        fptr_GetPrimaryModCount = (fptr_type_GetPrimaryModCount)GetProcAddress((HMODULE)handle, "GetPrimaryModCount");
        fptr_GetPrimaryModInfoCount = (fptr_type_GetPrimaryModInfoCount)GetProcAddress((HMODULE)handle, "GetPrimaryModInfoCount");
        fptr_GetPrimaryModArchive = (fptr_type_GetPrimaryModArchive)GetProcAddress((HMODULE)handle, "GetPrimaryModArchive");
        fptr_GetPrimaryModArchiveCount = (fptr_type_GetPrimaryModArchiveCount)GetProcAddress((HMODULE)handle, "GetPrimaryModArchiveCount");
        fptr_GetPrimaryModArchiveList = (fptr_type_GetPrimaryModArchiveList)GetProcAddress((HMODULE)handle, "GetPrimaryModArchiveList");
        fptr_GetPrimaryModIndex = (fptr_type_GetPrimaryModIndex)GetProcAddress((HMODULE)handle, "GetPrimaryModIndex");
        fptr_GetPrimaryModChecksum = (fptr_type_GetPrimaryModChecksum)GetProcAddress((HMODULE)handle, "GetPrimaryModChecksum");
        fptr_GetPrimaryModChecksumFromName = (fptr_type_GetPrimaryModChecksumFromName)GetProcAddress((HMODULE)handle, "GetPrimaryModChecksumFromName");
        fptr_GetSideCount = (fptr_type_GetSideCount)GetProcAddress((HMODULE)handle, "GetSideCount");
        fptr_GetSideName = (fptr_type_GetSideName)GetProcAddress((HMODULE)handle, "GetSideName");
        fptr_GetSideStartUnit = (fptr_type_GetSideStartUnit)GetProcAddress((HMODULE)handle, "GetSideStartUnit");
        fptr_GetMapOptionCount = (fptr_type_GetMapOptionCount)GetProcAddress((HMODULE)handle, "GetMapOptionCount");
        fptr_GetModOptionCount = (fptr_type_GetModOptionCount)GetProcAddress((HMODULE)handle, "GetModOptionCount");
        fptr_GetCustomOptionCount = (fptr_type_GetCustomOptionCount)GetProcAddress((HMODULE)handle, "GetCustomOptionCount");
        fptr_GetOptionKey = (fptr_type_GetOptionKey)GetProcAddress((HMODULE)handle, "GetOptionKey");
        fptr_GetOptionScope = (fptr_type_GetOptionScope)GetProcAddress((HMODULE)handle, "GetOptionScope");
        fptr_GetOptionName = (fptr_type_GetOptionName)GetProcAddress((HMODULE)handle, "GetOptionName");
        fptr_GetOptionSection = (fptr_type_GetOptionSection)GetProcAddress((HMODULE)handle, "GetOptionSection");
        fptr_GetOptionStyle = (fptr_type_GetOptionStyle)GetProcAddress((HMODULE)handle, "GetOptionStyle");
        fptr_GetOptionDesc = (fptr_type_GetOptionDesc)GetProcAddress((HMODULE)handle, "GetOptionDesc");
        fptr_GetOptionType = (fptr_type_GetOptionType)GetProcAddress((HMODULE)handle, "GetOptionType");
        fptr_GetOptionBoolDef = (fptr_type_GetOptionBoolDef)GetProcAddress((HMODULE)handle, "GetOptionBoolDef");
        fptr_GetOptionNumberDef = (fptr_type_GetOptionNumberDef)GetProcAddress((HMODULE)handle, "GetOptionNumberDef");
        fptr_GetOptionNumberMin = (fptr_type_GetOptionNumberMin)GetProcAddress((HMODULE)handle, "GetOptionNumberMin");
        fptr_GetOptionNumberMax = (fptr_type_GetOptionNumberMax)GetProcAddress((HMODULE)handle, "GetOptionNumberMax");
        fptr_GetOptionNumberStep = (fptr_type_GetOptionNumberStep)GetProcAddress((HMODULE)handle, "GetOptionNumberStep");
        fptr_GetOptionStringDef = (fptr_type_GetOptionStringDef)GetProcAddress((HMODULE)handle, "GetOptionStringDef");
        fptr_GetOptionStringMaxLen = (fptr_type_GetOptionStringMaxLen)GetProcAddress((HMODULE)handle, "GetOptionStringMaxLen");
        fptr_GetOptionListCount = (fptr_type_GetOptionListCount)GetProcAddress((HMODULE)handle, "GetOptionListCount");
        fptr_GetOptionListDef = (fptr_type_GetOptionListDef)GetProcAddress((HMODULE)handle, "GetOptionListDef");
        fptr_GetOptionListItemKey = (fptr_type_GetOptionListItemKey)GetProcAddress((HMODULE)handle, "GetOptionListItemKey");
        fptr_GetOptionListItemName = (fptr_type_GetOptionListItemName)GetProcAddress((HMODULE)handle, "GetOptionListItemName");
        fptr_GetOptionListItemDesc = (fptr_type_GetOptionListItemDesc)GetProcAddress((HMODULE)handle, "GetOptionListItemDesc");
        fptr_GetModValidMapCount = (fptr_type_GetModValidMapCount)GetProcAddress((HMODULE)handle, "GetModValidMapCount");
        fptr_GetModValidMap = (fptr_type_GetModValidMap)GetProcAddress((HMODULE)handle, "GetModValidMap");
        fptr_OpenFileVFS = (fptr_type_OpenFileVFS)GetProcAddress((HMODULE)handle, "OpenFileVFS");
        fptr_CloseFileVFS = (fptr_type_CloseFileVFS)GetProcAddress((HMODULE)handle, "CloseFileVFS");
        fptr_ReadFileVFS = (fptr_type_ReadFileVFS)GetProcAddress((HMODULE)handle, "ReadFileVFS");
        fptr_FileSizeVFS = (fptr_type_FileSizeVFS)GetProcAddress((HMODULE)handle, "FileSizeVFS");
        fptr_InitFindVFS = (fptr_type_InitFindVFS)GetProcAddress((HMODULE)handle, "InitFindVFS");
        fptr_InitDirListVFS = (fptr_type_InitDirListVFS)GetProcAddress((HMODULE)handle, "InitDirListVFS");
        fptr_InitSubDirsVFS = (fptr_type_InitSubDirsVFS)GetProcAddress((HMODULE)handle, "InitSubDirsVFS");
        fptr_FindFilesVFS = (fptr_type_FindFilesVFS)GetProcAddress((HMODULE)handle, "FindFilesVFS");
        fptr_OpenArchive = (fptr_type_OpenArchive)GetProcAddress((HMODULE)handle, "OpenArchive");
        fptr_CloseArchive = (fptr_type_CloseArchive)GetProcAddress((HMODULE)handle, "CloseArchive");
        fptr_FindFilesArchive = (fptr_type_FindFilesArchive)GetProcAddress((HMODULE)handle, "FindFilesArchive");
        fptr_OpenArchiveFile = (fptr_type_OpenArchiveFile)GetProcAddress((HMODULE)handle, "OpenArchiveFile");
        fptr_ReadArchiveFile = (fptr_type_ReadArchiveFile)GetProcAddress((HMODULE)handle, "ReadArchiveFile");
        fptr_CloseArchiveFile = (fptr_type_CloseArchiveFile)GetProcAddress((HMODULE)handle, "CloseArchiveFile");
        fptr_SizeArchiveFile = (fptr_type_SizeArchiveFile)GetProcAddress((HMODULE)handle, "SizeArchiveFile");
        fptr_SetSpringConfigFile = (fptr_type_SetSpringConfigFile)GetProcAddress((HMODULE)handle, "SetSpringConfigFile");
        fptr_GetSpringConfigFile = (fptr_type_GetSpringConfigFile)GetProcAddress((HMODULE)handle, "GetSpringConfigFile");
        fptr_GetSpringConfigString = (fptr_type_GetSpringConfigString)GetProcAddress((HMODULE)handle, "GetSpringConfigString");
        fptr_GetSpringConfigInt = (fptr_type_GetSpringConfigInt)GetProcAddress((HMODULE)handle, "GetSpringConfigInt");
        fptr_GetSpringConfigFloat = (fptr_type_GetSpringConfigFloat)GetProcAddress((HMODULE)handle, "GetSpringConfigFloat");
        fptr_SetSpringConfigString = (fptr_type_SetSpringConfigString)GetProcAddress((HMODULE)handle, "SetSpringConfigString");
        fptr_SetSpringConfigInt = (fptr_type_SetSpringConfigInt)GetProcAddress((HMODULE)handle, "SetSpringConfigInt");
        fptr_SetSpringConfigFloat = (fptr_type_SetSpringConfigFloat)GetProcAddress((HMODULE)handle, "SetSpringConfigFloat");
        fptr_DeleteSpringConfigKey = (fptr_type_DeleteSpringConfigKey)GetProcAddress((HMODULE)handle, "DeleteSpringConfigKey");
        fptr_lpClose = (fptr_type_lpClose)GetProcAddress((HMODULE)handle, "lpClose");
        fptr_lpOpenFile = (fptr_type_lpOpenFile)GetProcAddress((HMODULE)handle, "lpOpenFile");
        fptr_lpOpenSource = (fptr_type_lpOpenSource)GetProcAddress((HMODULE)handle, "lpOpenSource");
        fptr_lpExecute = (fptr_type_lpExecute)GetProcAddress((HMODULE)handle, "lpExecute");
        fptr_lpErrorLog = (fptr_type_lpErrorLog)GetProcAddress((HMODULE)handle, "lpErrorLog");
        fptr_lpAddTableInt = (fptr_type_lpAddTableInt)GetProcAddress((HMODULE)handle, "lpAddTableInt");
        fptr_lpAddTableStr = (fptr_type_lpAddTableStr)GetProcAddress((HMODULE)handle, "lpAddTableStr");
        fptr_lpEndTable = (fptr_type_lpEndTable)GetProcAddress((HMODULE)handle, "lpEndTable");
        fptr_lpAddIntKeyIntVal = (fptr_type_lpAddIntKeyIntVal)GetProcAddress((HMODULE)handle, "lpAddIntKeyIntVal");
        fptr_lpAddStrKeyIntVal = (fptr_type_lpAddStrKeyIntVal)GetProcAddress((HMODULE)handle, "lpAddStrKeyIntVal");
        fptr_lpAddIntKeyBoolVal = (fptr_type_lpAddIntKeyBoolVal)GetProcAddress((HMODULE)handle, "lpAddIntKeyBoolVal");
        fptr_lpAddStrKeyBoolVal = (fptr_type_lpAddStrKeyBoolVal)GetProcAddress((HMODULE)handle, "lpAddStrKeyBoolVal");
        fptr_lpAddIntKeyFloatVal = (fptr_type_lpAddIntKeyFloatVal)GetProcAddress((HMODULE)handle, "lpAddIntKeyFloatVal");
        fptr_lpAddStrKeyFloatVal = (fptr_type_lpAddStrKeyFloatVal)GetProcAddress((HMODULE)handle, "lpAddStrKeyFloatVal");
        fptr_lpAddIntKeyStrVal = (fptr_type_lpAddIntKeyStrVal)GetProcAddress((HMODULE)handle, "lpAddIntKeyStrVal");
        fptr_lpAddStrKeyStrVal = (fptr_type_lpAddStrKeyStrVal)GetProcAddress((HMODULE)handle, "lpAddStrKeyStrVal");
        fptr_lpRootTable = (fptr_type_lpRootTable)GetProcAddress((HMODULE)handle, "lpRootTable");
        fptr_lpRootTableExpr = (fptr_type_lpRootTableExpr)GetProcAddress((HMODULE)handle, "lpRootTableExpr");
        fptr_lpSubTableInt = (fptr_type_lpSubTableInt)GetProcAddress((HMODULE)handle, "lpSubTableInt");
        fptr_lpSubTableStr = (fptr_type_lpSubTableStr)GetProcAddress((HMODULE)handle, "lpSubTableStr");
        fptr_lpSubTableExpr = (fptr_type_lpSubTableExpr)GetProcAddress((HMODULE)handle, "lpSubTableExpr");
        fptr_lpPopTable = (fptr_type_lpPopTable)GetProcAddress((HMODULE)handle, "lpPopTable");
        fptr_lpGetKeyExistsInt = (fptr_type_lpGetKeyExistsInt)GetProcAddress((HMODULE)handle, "lpGetKeyExistsInt");
        fptr_lpGetKeyExistsStr = (fptr_type_lpGetKeyExistsStr)GetProcAddress((HMODULE)handle, "lpGetKeyExistsStr");
        fptr_lpGetIntKeyType = (fptr_type_lpGetIntKeyType)GetProcAddress((HMODULE)handle, "lpGetIntKeyType");
        fptr_lpGetStrKeyType = (fptr_type_lpGetStrKeyType)GetProcAddress((HMODULE)handle, "lpGetStrKeyType");
        fptr_lpGetIntKeyListCount = (fptr_type_lpGetIntKeyListCount)GetProcAddress((HMODULE)handle, "lpGetIntKeyListCount");
        fptr_lpGetIntKeyListEntry = (fptr_type_lpGetIntKeyListEntry)GetProcAddress((HMODULE)handle, "lpGetIntKeyListEntry");
        fptr_lpGetStrKeyListCount = (fptr_type_lpGetStrKeyListCount)GetProcAddress((HMODULE)handle, "lpGetStrKeyListCount");
        fptr_lpGetStrKeyListEntry = (fptr_type_lpGetStrKeyListEntry)GetProcAddress((HMODULE)handle, "lpGetStrKeyListEntry");
        fptr_lpGetIntKeyIntVal = (fptr_type_lpGetIntKeyIntVal)GetProcAddress((HMODULE)handle, "lpGetIntKeyIntVal");
        fptr_lpGetStrKeyIntVal = (fptr_type_lpGetStrKeyIntVal)GetProcAddress((HMODULE)handle, "lpGetStrKeyIntVal");
        fptr_lpGetIntKeyBoolVal = (fptr_type_lpGetIntKeyBoolVal)GetProcAddress((HMODULE)handle, "lpGetIntKeyBoolVal");
        fptr_lpGetStrKeyBoolVal = (fptr_type_lpGetStrKeyBoolVal)GetProcAddress((HMODULE)handle, "lpGetStrKeyBoolVal");
        fptr_lpGetIntKeyFloatVal = (fptr_type_lpGetIntKeyFloatVal)GetProcAddress((HMODULE)handle, "lpGetIntKeyFloatVal");
        fptr_lpGetStrKeyFloatVal = (fptr_type_lpGetStrKeyFloatVal)GetProcAddress((HMODULE)handle, "lpGetStrKeyFloatVal");
        fptr_lpGetIntKeyStrVal = (fptr_type_lpGetIntKeyStrVal)GetProcAddress((HMODULE)handle, "lpGetIntKeyStrVal");
        fptr_lpGetStrKeyStrVal = (fptr_type_lpGetStrKeyStrVal)GetProcAddress((HMODULE)handle, "lpGetStrKeyStrVal");
        fptr_ProcessUnitsNoChecksum = (fptr_type_ProcessUnitsNoChecksum)GetProcAddress((HMODULE)handle, "ProcessUnitsNoChecksum");
        fptr_GetMapInfoEx = (fptr_type_GetMapInfoEx)GetProcAddress((HMODULE)handle, "GetMapInfoEx");
        fptr_GetMapInfo = (fptr_type_GetMapInfo)GetProcAddress((HMODULE)handle, "GetMapInfo");
        fptr_GetInfoValue = (fptr_type_GetInfoValue)GetProcAddress((HMODULE)handle, "GetInfoValue");
        fptr_GetPrimaryModName = (fptr_type_GetPrimaryModName)GetProcAddress((HMODULE)handle, "GetPrimaryModName");
        fptr_GetPrimaryModShortName = (fptr_type_GetPrimaryModShortName)GetProcAddress((HMODULE)handle, "GetPrimaryModShortName");
        fptr_GetPrimaryModVersion = (fptr_type_GetPrimaryModVersion)GetProcAddress((HMODULE)handle, "GetPrimaryModVersion");
        fptr_GetPrimaryModMutator = (fptr_type_GetPrimaryModMutator)GetProcAddress((HMODULE)handle, "GetPrimaryModMutator");
        fptr_GetPrimaryModGame = (fptr_type_GetPrimaryModGame)GetProcAddress((HMODULE)handle, "GetPrimaryModGame");
        fptr_GetPrimaryModShortGame = (fptr_type_GetPrimaryModShortGame)GetProcAddress((HMODULE)handle, "GetPrimaryModShortGame");
        fptr_GetPrimaryModDescription = (fptr_type_GetPrimaryModDescription)GetProcAddress((HMODULE)handle, "GetPrimaryModDescription");
        fptr_OpenArchiveType = (fptr_type_OpenArchiveType)GetProcAddress((HMODULE)handle, "OpenArchiveType");

        ready = true;
    #else
        #error "Unknown target OS."
    #endif
}

UnitsyncHandlerAsync::~UnitsyncHandlerAsync() {{
        boost::unique_lock<boost::mutex> lock(queueMutex);
        while (!queue.empty())
            queue.pop();
        ready = false;
    }
    queueCond.notify_all();
    if (workThread.joinable())
	workThread.join();

    #if defined Q_OS_LINUX || defined Q_OS_MAC
        if (handle)
            dlclose(handle);
    #elif defined Q_OS_WIN32
        if (handle)
            FreeLibrary((HMODULE)handle);
    #else
        #error "Unknown target OS."
    #endif
}

UnitsyncHandlerAsync::UnitsyncHandlerAsync(UnitsyncHandlerAsync&& h) : QObject(h.parent()),
        logger(h.logger), ready(h.ready), handle(h.handle), queue(std::move(h.queue)) {

    h.handle = NULL; // Quite an important line, if you ask me.

    fptr_GetNextError = h.fptr_GetNextError;
    fptr_GetSpringVersion = h.fptr_GetSpringVersion;
    fptr_GetSpringVersionPatchset = h.fptr_GetSpringVersionPatchset;
    fptr_IsSpringReleaseVersion = h.fptr_IsSpringReleaseVersion;
    fptr_Init = h.fptr_Init;
    fptr_UnInit = h.fptr_UnInit;
    fptr_GetWritableDataDirectory = h.fptr_GetWritableDataDirectory;
    fptr_GetDataDirectoryCount = h.fptr_GetDataDirectoryCount;
    fptr_GetDataDirectory = h.fptr_GetDataDirectory;
    fptr_ProcessUnits = h.fptr_ProcessUnits;
    fptr_GetUnitCount = h.fptr_GetUnitCount;
    fptr_GetUnitName = h.fptr_GetUnitName;
    fptr_GetFullUnitName = h.fptr_GetFullUnitName;
    fptr_AddArchive = h.fptr_AddArchive;
    fptr_AddAllArchives = h.fptr_AddAllArchives;
    fptr_RemoveAllArchives = h.fptr_RemoveAllArchives;
    fptr_GetArchiveChecksum = h.fptr_GetArchiveChecksum;
    fptr_GetArchivePath = h.fptr_GetArchivePath;
    fptr_GetMapCount = h.fptr_GetMapCount;
    fptr_GetMapName = h.fptr_GetMapName;
    fptr_GetMapFileName = h.fptr_GetMapFileName;
    fptr_GetMapDescription = h.fptr_GetMapDescription;
    fptr_GetMapAuthor = h.fptr_GetMapAuthor;
    fptr_GetMapWidth = h.fptr_GetMapWidth;
    fptr_GetMapHeight = h.fptr_GetMapHeight;
    fptr_GetMapTidalStrength = h.fptr_GetMapTidalStrength;
    fptr_GetMapWindMin = h.fptr_GetMapWindMin;
    fptr_GetMapWindMax = h.fptr_GetMapWindMax;
    fptr_GetMapGravity = h.fptr_GetMapGravity;
    fptr_GetMapResourceCount = h.fptr_GetMapResourceCount;
    fptr_GetMapResourceName = h.fptr_GetMapResourceName;
    fptr_GetMapResourceMax = h.fptr_GetMapResourceMax;
    fptr_GetMapResourceExtractorRadius = h.fptr_GetMapResourceExtractorRadius;
    fptr_GetMapPosCount = h.fptr_GetMapPosCount;
    fptr_GetMapPosX = h.fptr_GetMapPosX;
    fptr_GetMapPosZ = h.fptr_GetMapPosZ;
    fptr_GetMapMinHeight = h.fptr_GetMapMinHeight;
    fptr_GetMapMaxHeight = h.fptr_GetMapMaxHeight;
    fptr_GetMapArchiveCount = h.fptr_GetMapArchiveCount;
    fptr_GetMapArchiveName = h.fptr_GetMapArchiveName;
    fptr_GetMapChecksum = h.fptr_GetMapChecksum;
    fptr_GetMapChecksumFromName = h.fptr_GetMapChecksumFromName;
    fptr_GetMinimap = h.fptr_GetMinimap;
    fptr_GetInfoMapSize = h.fptr_GetInfoMapSize;
    fptr_GetInfoMap = h.fptr_GetInfoMap;
    fptr_GetSkirmishAICount = h.fptr_GetSkirmishAICount;
    fptr_GetSkirmishAIInfoCount = h.fptr_GetSkirmishAIInfoCount;
    fptr_GetInfoKey = h.fptr_GetInfoKey;
    fptr_GetInfoType = h.fptr_GetInfoType;
    fptr_GetInfoValueString = h.fptr_GetInfoValueString;
    fptr_GetInfoValueInteger = h.fptr_GetInfoValueInteger;
    fptr_GetInfoValueFloat = h.fptr_GetInfoValueFloat;
    fptr_GetInfoValueBool = h.fptr_GetInfoValueBool;
    fptr_GetInfoDescription = h.fptr_GetInfoDescription;
    fptr_GetSkirmishAIOptionCount = h.fptr_GetSkirmishAIOptionCount;
    fptr_GetPrimaryModCount = h.fptr_GetPrimaryModCount;
    fptr_GetPrimaryModInfoCount = h.fptr_GetPrimaryModInfoCount;
    fptr_GetPrimaryModArchive = h.fptr_GetPrimaryModArchive;
    fptr_GetPrimaryModArchiveCount = h.fptr_GetPrimaryModArchiveCount;
    fptr_GetPrimaryModArchiveList = h.fptr_GetPrimaryModArchiveList;
    fptr_GetPrimaryModIndex = h.fptr_GetPrimaryModIndex;
    fptr_GetPrimaryModChecksum = h.fptr_GetPrimaryModChecksum;
    fptr_GetPrimaryModChecksumFromName = h.fptr_GetPrimaryModChecksumFromName;
    fptr_GetSideCount = h.fptr_GetSideCount;
    fptr_GetSideName = h.fptr_GetSideName;
    fptr_GetSideStartUnit = h.fptr_GetSideStartUnit;
    fptr_GetMapOptionCount = h.fptr_GetMapOptionCount;
    fptr_GetModOptionCount = h.fptr_GetModOptionCount;
    fptr_GetCustomOptionCount = h.fptr_GetCustomOptionCount;
    fptr_GetOptionKey = h.fptr_GetOptionKey;
    fptr_GetOptionScope = h.fptr_GetOptionScope;
    fptr_GetOptionName = h.fptr_GetOptionName;
    fptr_GetOptionSection = h.fptr_GetOptionSection;
    fptr_GetOptionStyle = h.fptr_GetOptionStyle;
    fptr_GetOptionDesc = h.fptr_GetOptionDesc;
    fptr_GetOptionType = h.fptr_GetOptionType;
    fptr_GetOptionBoolDef = h.fptr_GetOptionBoolDef;
    fptr_GetOptionNumberDef = h.fptr_GetOptionNumberDef;
    fptr_GetOptionNumberMin = h.fptr_GetOptionNumberMin;
    fptr_GetOptionNumberMax = h.fptr_GetOptionNumberMax;
    fptr_GetOptionNumberStep = h.fptr_GetOptionNumberStep;
    fptr_GetOptionStringDef = h.fptr_GetOptionStringDef;
    fptr_GetOptionStringMaxLen = h.fptr_GetOptionStringMaxLen;
    fptr_GetOptionListCount = h.fptr_GetOptionListCount;
    fptr_GetOptionListDef = h.fptr_GetOptionListDef;
    fptr_GetOptionListItemKey = h.fptr_GetOptionListItemKey;
    fptr_GetOptionListItemName = h.fptr_GetOptionListItemName;
    fptr_GetOptionListItemDesc = h.fptr_GetOptionListItemDesc;
    fptr_GetModValidMapCount = h.fptr_GetModValidMapCount;
    fptr_GetModValidMap = h.fptr_GetModValidMap;
    fptr_OpenFileVFS = h.fptr_OpenFileVFS;
    fptr_CloseFileVFS = h.fptr_CloseFileVFS;
    fptr_ReadFileVFS = h.fptr_ReadFileVFS;
    fptr_FileSizeVFS = h.fptr_FileSizeVFS;
    fptr_InitFindVFS = h.fptr_InitFindVFS;
    fptr_InitDirListVFS = h.fptr_InitDirListVFS;
    fptr_InitSubDirsVFS = h.fptr_InitSubDirsVFS;
    fptr_FindFilesVFS = h.fptr_FindFilesVFS;
    fptr_OpenArchive = h.fptr_OpenArchive;
    fptr_CloseArchive = h.fptr_CloseArchive;
    fptr_FindFilesArchive = h.fptr_FindFilesArchive;
    fptr_OpenArchiveFile = h.fptr_OpenArchiveFile;
    fptr_ReadArchiveFile = h.fptr_ReadArchiveFile;
    fptr_CloseArchiveFile = h.fptr_CloseArchiveFile;
    fptr_SizeArchiveFile = h.fptr_SizeArchiveFile;
    fptr_SetSpringConfigFile = h.fptr_SetSpringConfigFile;
    fptr_GetSpringConfigFile = h.fptr_GetSpringConfigFile;
    fptr_GetSpringConfigString = h.fptr_GetSpringConfigString;
    fptr_GetSpringConfigInt = h.fptr_GetSpringConfigInt;
    fptr_GetSpringConfigFloat = h.fptr_GetSpringConfigFloat;
    fptr_SetSpringConfigString = h.fptr_SetSpringConfigString;
    fptr_SetSpringConfigInt = h.fptr_SetSpringConfigInt;
    fptr_SetSpringConfigFloat = h.fptr_SetSpringConfigFloat;
    fptr_DeleteSpringConfigKey = h.fptr_DeleteSpringConfigKey;
    fptr_lpClose = h.fptr_lpClose;
    fptr_lpOpenFile = h.fptr_lpOpenFile;
    fptr_lpOpenSource = h.fptr_lpOpenSource;
    fptr_lpExecute = h.fptr_lpExecute;
    fptr_lpErrorLog = h.fptr_lpErrorLog;
    fptr_lpAddTableInt = h.fptr_lpAddTableInt;
    fptr_lpAddTableStr = h.fptr_lpAddTableStr;
    fptr_lpEndTable = h.fptr_lpEndTable;
    fptr_lpAddIntKeyIntVal = h.fptr_lpAddIntKeyIntVal;
    fptr_lpAddStrKeyIntVal = h.fptr_lpAddStrKeyIntVal;
    fptr_lpAddIntKeyBoolVal = h.fptr_lpAddIntKeyBoolVal;
    fptr_lpAddStrKeyBoolVal = h.fptr_lpAddStrKeyBoolVal;
    fptr_lpAddIntKeyFloatVal = h.fptr_lpAddIntKeyFloatVal;
    fptr_lpAddStrKeyFloatVal = h.fptr_lpAddStrKeyFloatVal;
    fptr_lpAddIntKeyStrVal = h.fptr_lpAddIntKeyStrVal;
    fptr_lpAddStrKeyStrVal = h.fptr_lpAddStrKeyStrVal;
    fptr_lpRootTable = h.fptr_lpRootTable;
    fptr_lpRootTableExpr = h.fptr_lpRootTableExpr;
    fptr_lpSubTableInt = h.fptr_lpSubTableInt;
    fptr_lpSubTableStr = h.fptr_lpSubTableStr;
    fptr_lpSubTableExpr = h.fptr_lpSubTableExpr;
    fptr_lpPopTable = h.fptr_lpPopTable;
    fptr_lpGetKeyExistsInt = h.fptr_lpGetKeyExistsInt;
    fptr_lpGetKeyExistsStr = h.fptr_lpGetKeyExistsStr;
    fptr_lpGetIntKeyType = h.fptr_lpGetIntKeyType;
    fptr_lpGetStrKeyType = h.fptr_lpGetStrKeyType;
    fptr_lpGetIntKeyListCount = h.fptr_lpGetIntKeyListCount;
    fptr_lpGetIntKeyListEntry = h.fptr_lpGetIntKeyListEntry;
    fptr_lpGetStrKeyListCount = h.fptr_lpGetStrKeyListCount;
    fptr_lpGetStrKeyListEntry = h.fptr_lpGetStrKeyListEntry;
    fptr_lpGetIntKeyIntVal = h.fptr_lpGetIntKeyIntVal;
    fptr_lpGetStrKeyIntVal = h.fptr_lpGetStrKeyIntVal;
    fptr_lpGetIntKeyBoolVal = h.fptr_lpGetIntKeyBoolVal;
    fptr_lpGetStrKeyBoolVal = h.fptr_lpGetStrKeyBoolVal;
    fptr_lpGetIntKeyFloatVal = h.fptr_lpGetIntKeyFloatVal;
    fptr_lpGetStrKeyFloatVal = h.fptr_lpGetStrKeyFloatVal;
    fptr_lpGetIntKeyStrVal = h.fptr_lpGetIntKeyStrVal;
    fptr_lpGetStrKeyStrVal = h.fptr_lpGetStrKeyStrVal;
    fptr_ProcessUnitsNoChecksum = h.fptr_ProcessUnitsNoChecksum;
    fptr_GetMapInfoEx = h.fptr_GetMapInfoEx;
    fptr_GetMapInfo = h.fptr_GetMapInfo;
    fptr_GetInfoValue = h.fptr_GetInfoValue;
    fptr_GetPrimaryModName = h.fptr_GetPrimaryModName;
    fptr_GetPrimaryModShortName = h.fptr_GetPrimaryModShortName;
    fptr_GetPrimaryModVersion = h.fptr_GetPrimaryModVersion;
    fptr_GetPrimaryModMutator = h.fptr_GetPrimaryModMutator;
    fptr_GetPrimaryModGame = h.fptr_GetPrimaryModGame;
    fptr_GetPrimaryModShortGame = h.fptr_GetPrimaryModShortGame;
    fptr_GetPrimaryModDescription = h.fptr_GetPrimaryModDescription;
    fptr_OpenArchiveType = h.fptr_OpenArchiveType;
}

// Returns a %-escaped string ready for use in a data URL.
void UnitsyncHandlerAsync::jsReadFileVFS(QString __id, int fd, int size) {
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call jsReadFileVFS(", fd, ", ", size, ")");
        // An astute reader might ask at this point "But ikinz, why in the world do you need that offset?"
        // It turns out that on mingw from mingw-builds v4.8.1-posix-dwarf-rev5 using qt-5.2.0 in this very
        // functions suddenly appears an unknown and unstoppable force that overwrites the first few bytes
        // of readBuf with foul garbage. I spent a day in vain trying to discern its origins, but the force
        // evaded the grasp of my sleep-deprived mind and humored me when I tried to allocate readBuf
        // dynamically by causing a segfault in metacall. So I decided to submit to it and let it overwrite
        // my buffers all it wants as long as it stays within the boundaries set by the offset.
        //
        // On a more serious note that's likely some obscure moc bug that also probably has something with
        // threading since it segfaulted in a different thread. Whatever, faction icons seem to work on
        // Windows now so I'm happy.
        const int off = 100;
        unsigned char readBuf[size + off];
        if (fptr_ReadFileVFS(fd, readBuf + off, size) != size)
            logger.warning("ReadFileVFS(): size mismatch");
        std::string res;
        char tmp[8];
        for (int i = 0; i < size; i++) {
            std::snprintf(tmp, 8, "%%%.2hhX", readBuf[i + off]);
            res += tmp;
        }
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}


void UnitsyncHandlerAsync::getNextError(QString __id) {
    if (fptr_GetNextError == NULL) {
        logger.error("Bad function pointer: GetNextError");
        throw bad_fptr("GetNextError");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetNextError(", ")");
        const char* res = fptr_GetNextError();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSpringVersion(QString __id) {
    if (fptr_GetSpringVersion == NULL) {
        logger.error("Bad function pointer: GetSpringVersion");
        throw bad_fptr("GetSpringVersion");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSpringVersion(", ")");
        const char* res = fptr_GetSpringVersion();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSpringVersionPatchset(QString __id) {
    if (fptr_GetSpringVersionPatchset == NULL) {
        logger.error("Bad function pointer: GetSpringVersionPatchset");
        throw bad_fptr("GetSpringVersionPatchset");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSpringVersionPatchset(", ")");
        const char* res = fptr_GetSpringVersionPatchset();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::isSpringReleaseVersion(QString __id) {
    if (fptr_IsSpringReleaseVersion == NULL) {
        logger.error("Bad function pointer: IsSpringReleaseVersion");
        throw bad_fptr("IsSpringReleaseVersion");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call IsSpringReleaseVersion(", ")");
        bool res = fptr_IsSpringReleaseVersion();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "bool", (res ? "true" : "false")));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::init(QString __id, bool isServer, int id) {
    if (fptr_Init == NULL) {
        logger.error("Bad function pointer: Init");
        throw bad_fptr("Init");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call Init(", isServer, ", ", id, ")");
        int res = fptr_Init(isServer, id);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::unInit(QString __id) {
    if (fptr_UnInit == NULL) {
        logger.error("Bad function pointer: UnInit");
        throw bad_fptr("UnInit");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call UnInit(", ")");
        fptr_UnInit();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getWritableDataDirectory(QString __id) {
    if (fptr_GetWritableDataDirectory == NULL) {
        logger.error("Bad function pointer: GetWritableDataDirectory");
        throw bad_fptr("GetWritableDataDirectory");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetWritableDataDirectory(", ")");
        const char* res = fptr_GetWritableDataDirectory();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getDataDirectoryCount(QString __id) {
    if (fptr_GetDataDirectoryCount == NULL) {
        logger.error("Bad function pointer: GetDataDirectoryCount");
        throw bad_fptr("GetDataDirectoryCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetDataDirectoryCount(", ")");
        int res = fptr_GetDataDirectoryCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getDataDirectory(QString __id, int index) {
    if (fptr_GetDataDirectory == NULL) {
        logger.error("Bad function pointer: GetDataDirectory");
        throw bad_fptr("GetDataDirectory");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetDataDirectory(", index, ")");
        const char* res = fptr_GetDataDirectory(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::processUnits(QString __id) {
    if (fptr_ProcessUnits == NULL) {
        logger.error("Bad function pointer: ProcessUnits");
        throw bad_fptr("ProcessUnits");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call ProcessUnits(", ")");
        int res = fptr_ProcessUnits();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getUnitCount(QString __id) {
    if (fptr_GetUnitCount == NULL) {
        logger.error("Bad function pointer: GetUnitCount");
        throw bad_fptr("GetUnitCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetUnitCount(", ")");
        int res = fptr_GetUnitCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getUnitName(QString __id, int unit) {
    if (fptr_GetUnitName == NULL) {
        logger.error("Bad function pointer: GetUnitName");
        throw bad_fptr("GetUnitName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetUnitName(", unit, ")");
        const char* res = fptr_GetUnitName(unit);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getFullUnitName(QString __id, int unit) {
    if (fptr_GetFullUnitName == NULL) {
        logger.error("Bad function pointer: GetFullUnitName");
        throw bad_fptr("GetFullUnitName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetFullUnitName(", unit, ")");
        const char* res = fptr_GetFullUnitName(unit);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::addArchive(QString __id, QString archiveName) {
    if (fptr_AddArchive == NULL) {
        logger.error("Bad function pointer: AddArchive");
        throw bad_fptr("AddArchive");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call AddArchive(", archiveName.toStdString().c_str(), ")");
        fptr_AddArchive(archiveName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::addAllArchives(QString __id, QString rootArchiveName) {
    if (fptr_AddAllArchives == NULL) {
        logger.error("Bad function pointer: AddAllArchives");
        throw bad_fptr("AddAllArchives");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call AddAllArchives(", rootArchiveName.toStdString().c_str(), ")");
        fptr_AddAllArchives(rootArchiveName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::removeAllArchives(QString __id) {
    if (fptr_RemoveAllArchives == NULL) {
        logger.error("Bad function pointer: RemoveAllArchives");
        throw bad_fptr("RemoveAllArchives");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call RemoveAllArchives(", ")");
        fptr_RemoveAllArchives();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getArchiveChecksum(QString __id, QString archiveName) {
    if (fptr_GetArchiveChecksum == NULL) {
        logger.error("Bad function pointer: GetArchiveChecksum");
        throw bad_fptr("GetArchiveChecksum");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetArchiveChecksum(", archiveName.toStdString().c_str(), ")");
        unsigned int res = fptr_GetArchiveChecksum(archiveName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "unsigned int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getArchivePath(QString __id, QString archiveName) {
    if (fptr_GetArchivePath == NULL) {
        logger.error("Bad function pointer: GetArchivePath");
        throw bad_fptr("GetArchivePath");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetArchivePath(", archiveName.toStdString().c_str(), ")");
        const char* res = fptr_GetArchivePath(archiveName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapCount(QString __id) {
    if (fptr_GetMapCount == NULL) {
        logger.error("Bad function pointer: GetMapCount");
        throw bad_fptr("GetMapCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapCount(", ")");
        int res = fptr_GetMapCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapName(QString __id, int index) {
    if (fptr_GetMapName == NULL) {
        logger.error("Bad function pointer: GetMapName");
        throw bad_fptr("GetMapName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapName(", index, ")");
        const char* res = fptr_GetMapName(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapFileName(QString __id, int index) {
    if (fptr_GetMapFileName == NULL) {
        logger.error("Bad function pointer: GetMapFileName");
        throw bad_fptr("GetMapFileName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapFileName(", index, ")");
        const char* res = fptr_GetMapFileName(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapDescription(QString __id, int index) {
    if (fptr_GetMapDescription == NULL) {
        logger.error("Bad function pointer: GetMapDescription");
        throw bad_fptr("GetMapDescription");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapDescription(", index, ")");
        const char* res = fptr_GetMapDescription(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapAuthor(QString __id, int index) {
    if (fptr_GetMapAuthor == NULL) {
        logger.error("Bad function pointer: GetMapAuthor");
        throw bad_fptr("GetMapAuthor");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapAuthor(", index, ")");
        const char* res = fptr_GetMapAuthor(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapWidth(QString __id, int index) {
    if (fptr_GetMapWidth == NULL) {
        logger.error("Bad function pointer: GetMapWidth");
        throw bad_fptr("GetMapWidth");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapWidth(", index, ")");
        int res = fptr_GetMapWidth(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapHeight(QString __id, int index) {
    if (fptr_GetMapHeight == NULL) {
        logger.error("Bad function pointer: GetMapHeight");
        throw bad_fptr("GetMapHeight");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapHeight(", index, ")");
        int res = fptr_GetMapHeight(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapTidalStrength(QString __id, int index) {
    if (fptr_GetMapTidalStrength == NULL) {
        logger.error("Bad function pointer: GetMapTidalStrength");
        throw bad_fptr("GetMapTidalStrength");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapTidalStrength(", index, ")");
        int res = fptr_GetMapTidalStrength(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapWindMin(QString __id, int index) {
    if (fptr_GetMapWindMin == NULL) {
        logger.error("Bad function pointer: GetMapWindMin");
        throw bad_fptr("GetMapWindMin");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapWindMin(", index, ")");
        int res = fptr_GetMapWindMin(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapWindMax(QString __id, int index) {
    if (fptr_GetMapWindMax == NULL) {
        logger.error("Bad function pointer: GetMapWindMax");
        throw bad_fptr("GetMapWindMax");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapWindMax(", index, ")");
        int res = fptr_GetMapWindMax(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapGravity(QString __id, int index) {
    if (fptr_GetMapGravity == NULL) {
        logger.error("Bad function pointer: GetMapGravity");
        throw bad_fptr("GetMapGravity");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapGravity(", index, ")");
        int res = fptr_GetMapGravity(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapResourceCount(QString __id, int index) {
    if (fptr_GetMapResourceCount == NULL) {
        logger.error("Bad function pointer: GetMapResourceCount");
        throw bad_fptr("GetMapResourceCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapResourceCount(", index, ")");
        int res = fptr_GetMapResourceCount(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapResourceName(QString __id, int index, int resourceIndex) {
    if (fptr_GetMapResourceName == NULL) {
        logger.error("Bad function pointer: GetMapResourceName");
        throw bad_fptr("GetMapResourceName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapResourceName(", index, ", ", resourceIndex, ")");
        const char* res = fptr_GetMapResourceName(index, resourceIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapResourceMax(QString __id, int index, int resourceIndex) {
    if (fptr_GetMapResourceMax == NULL) {
        logger.error("Bad function pointer: GetMapResourceMax");
        throw bad_fptr("GetMapResourceMax");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapResourceMax(", index, ", ", resourceIndex, ")");
        float res = fptr_GetMapResourceMax(index, resourceIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapResourceExtractorRadius(QString __id, int index, int resourceIndex) {
    if (fptr_GetMapResourceExtractorRadius == NULL) {
        logger.error("Bad function pointer: GetMapResourceExtractorRadius");
        throw bad_fptr("GetMapResourceExtractorRadius");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapResourceExtractorRadius(", index, ", ", resourceIndex, ")");
        int res = fptr_GetMapResourceExtractorRadius(index, resourceIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapPosCount(QString __id, int index) {
    if (fptr_GetMapPosCount == NULL) {
        logger.error("Bad function pointer: GetMapPosCount");
        throw bad_fptr("GetMapPosCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapPosCount(", index, ")");
        int res = fptr_GetMapPosCount(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapPosX(QString __id, int index, int posIndex) {
    if (fptr_GetMapPosX == NULL) {
        logger.error("Bad function pointer: GetMapPosX");
        throw bad_fptr("GetMapPosX");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapPosX(", index, ", ", posIndex, ")");
        float res = fptr_GetMapPosX(index, posIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapPosZ(QString __id, int index, int posIndex) {
    if (fptr_GetMapPosZ == NULL) {
        logger.error("Bad function pointer: GetMapPosZ");
        throw bad_fptr("GetMapPosZ");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapPosZ(", index, ", ", posIndex, ")");
        float res = fptr_GetMapPosZ(index, posIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapMinHeight(QString __id, QString mapName) {
    if (fptr_GetMapMinHeight == NULL) {
        logger.error("Bad function pointer: GetMapMinHeight");
        throw bad_fptr("GetMapMinHeight");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapMinHeight(", mapName.toStdString().c_str(), ")");
        float res = fptr_GetMapMinHeight(mapName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapMaxHeight(QString __id, QString mapName) {
    if (fptr_GetMapMaxHeight == NULL) {
        logger.error("Bad function pointer: GetMapMaxHeight");
        throw bad_fptr("GetMapMaxHeight");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapMaxHeight(", mapName.toStdString().c_str(), ")");
        float res = fptr_GetMapMaxHeight(mapName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapArchiveCount(QString __id, QString mapName) {
    if (fptr_GetMapArchiveCount == NULL) {
        logger.error("Bad function pointer: GetMapArchiveCount");
        throw bad_fptr("GetMapArchiveCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapArchiveCount(", mapName.toStdString().c_str(), ")");
        int res = fptr_GetMapArchiveCount(mapName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapArchiveName(QString __id, int index) {
    if (fptr_GetMapArchiveName == NULL) {
        logger.error("Bad function pointer: GetMapArchiveName");
        throw bad_fptr("GetMapArchiveName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapArchiveName(", index, ")");
        const char* res = fptr_GetMapArchiveName(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapChecksum(QString __id, int index) {
    if (fptr_GetMapChecksum == NULL) {
        logger.error("Bad function pointer: GetMapChecksum");
        throw bad_fptr("GetMapChecksum");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapChecksum(", index, ")");
        unsigned int res = fptr_GetMapChecksum(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "unsigned int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapChecksumFromName(QString __id, QString mapName) {
    if (fptr_GetMapChecksumFromName == NULL) {
        logger.error("Bad function pointer: GetMapChecksumFromName");
        throw bad_fptr("GetMapChecksumFromName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapChecksumFromName(", mapName.toStdString().c_str(), ")");
        unsigned int res = fptr_GetMapChecksumFromName(mapName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "unsigned int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSkirmishAICount(QString __id) {
    if (fptr_GetSkirmishAICount == NULL) {
        logger.error("Bad function pointer: GetSkirmishAICount");
        throw bad_fptr("GetSkirmishAICount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSkirmishAICount(", ")");
        int res = fptr_GetSkirmishAICount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSkirmishAIInfoCount(QString __id, int index) {
    if (fptr_GetSkirmishAIInfoCount == NULL) {
        logger.error("Bad function pointer: GetSkirmishAIInfoCount");
        throw bad_fptr("GetSkirmishAIInfoCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSkirmishAIInfoCount(", index, ")");
        int res = fptr_GetSkirmishAIInfoCount(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getInfoKey(QString __id, int index) {
    if (fptr_GetInfoKey == NULL) {
        logger.error("Bad function pointer: GetInfoKey");
        throw bad_fptr("GetInfoKey");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetInfoKey(", index, ")");
        const char* res = fptr_GetInfoKey(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getInfoType(QString __id, int index) {
    if (fptr_GetInfoType == NULL) {
        logger.error("Bad function pointer: GetInfoType");
        throw bad_fptr("GetInfoType");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetInfoType(", index, ")");
        const char* res = fptr_GetInfoType(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getInfoValueString(QString __id, int index) {
    if (fptr_GetInfoValueString == NULL) {
        logger.error("Bad function pointer: GetInfoValueString");
        throw bad_fptr("GetInfoValueString");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetInfoValueString(", index, ")");
        const char* res = fptr_GetInfoValueString(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getInfoValueInteger(QString __id, int index) {
    if (fptr_GetInfoValueInteger == NULL) {
        logger.error("Bad function pointer: GetInfoValueInteger");
        throw bad_fptr("GetInfoValueInteger");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetInfoValueInteger(", index, ")");
        int res = fptr_GetInfoValueInteger(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getInfoValueFloat(QString __id, int index) {
    if (fptr_GetInfoValueFloat == NULL) {
        logger.error("Bad function pointer: GetInfoValueFloat");
        throw bad_fptr("GetInfoValueFloat");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetInfoValueFloat(", index, ")");
        float res = fptr_GetInfoValueFloat(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getInfoValueBool(QString __id, int index) {
    if (fptr_GetInfoValueBool == NULL) {
        logger.error("Bad function pointer: GetInfoValueBool");
        throw bad_fptr("GetInfoValueBool");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetInfoValueBool(", index, ")");
        bool res = fptr_GetInfoValueBool(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "bool", (res ? "true" : "false")));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getInfoDescription(QString __id, int index) {
    if (fptr_GetInfoDescription == NULL) {
        logger.error("Bad function pointer: GetInfoDescription");
        throw bad_fptr("GetInfoDescription");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetInfoDescription(", index, ")");
        const char* res = fptr_GetInfoDescription(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSkirmishAIOptionCount(QString __id, int index) {
    if (fptr_GetSkirmishAIOptionCount == NULL) {
        logger.error("Bad function pointer: GetSkirmishAIOptionCount");
        throw bad_fptr("GetSkirmishAIOptionCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSkirmishAIOptionCount(", index, ")");
        int res = fptr_GetSkirmishAIOptionCount(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModCount(QString __id) {
    if (fptr_GetPrimaryModCount == NULL) {
        logger.error("Bad function pointer: GetPrimaryModCount");
        throw bad_fptr("GetPrimaryModCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModCount(", ")");
        int res = fptr_GetPrimaryModCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModInfoCount(QString __id, int index) {
    if (fptr_GetPrimaryModInfoCount == NULL) {
        logger.error("Bad function pointer: GetPrimaryModInfoCount");
        throw bad_fptr("GetPrimaryModInfoCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModInfoCount(", index, ")");
        int res = fptr_GetPrimaryModInfoCount(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModArchive(QString __id, int index) {
    if (fptr_GetPrimaryModArchive == NULL) {
        logger.error("Bad function pointer: GetPrimaryModArchive");
        throw bad_fptr("GetPrimaryModArchive");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModArchive(", index, ")");
        const char* res = fptr_GetPrimaryModArchive(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModArchiveCount(QString __id, int index) {
    if (fptr_GetPrimaryModArchiveCount == NULL) {
        logger.error("Bad function pointer: GetPrimaryModArchiveCount");
        throw bad_fptr("GetPrimaryModArchiveCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModArchiveCount(", index, ")");
        int res = fptr_GetPrimaryModArchiveCount(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModArchiveList(QString __id, int archive) {
    if (fptr_GetPrimaryModArchiveList == NULL) {
        logger.error("Bad function pointer: GetPrimaryModArchiveList");
        throw bad_fptr("GetPrimaryModArchiveList");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModArchiveList(", archive, ")");
        const char* res = fptr_GetPrimaryModArchiveList(archive);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModIndex(QString __id, QString name) {
    if (fptr_GetPrimaryModIndex == NULL) {
        logger.error("Bad function pointer: GetPrimaryModIndex");
        throw bad_fptr("GetPrimaryModIndex");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModIndex(", name.toStdString().c_str(), ")");
        int res = fptr_GetPrimaryModIndex(name.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModChecksum(QString __id, int index) {
    if (fptr_GetPrimaryModChecksum == NULL) {
        logger.error("Bad function pointer: GetPrimaryModChecksum");
        throw bad_fptr("GetPrimaryModChecksum");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModChecksum(", index, ")");
        unsigned int res = fptr_GetPrimaryModChecksum(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "unsigned int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModChecksumFromName(QString __id, QString name) {
    if (fptr_GetPrimaryModChecksumFromName == NULL) {
        logger.error("Bad function pointer: GetPrimaryModChecksumFromName");
        throw bad_fptr("GetPrimaryModChecksumFromName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModChecksumFromName(", name.toStdString().c_str(), ")");
        unsigned int res = fptr_GetPrimaryModChecksumFromName(name.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "unsigned int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSideCount(QString __id) {
    if (fptr_GetSideCount == NULL) {
        logger.error("Bad function pointer: GetSideCount");
        throw bad_fptr("GetSideCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSideCount(", ")");
        int res = fptr_GetSideCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSideName(QString __id, int side) {
    if (fptr_GetSideName == NULL) {
        logger.error("Bad function pointer: GetSideName");
        throw bad_fptr("GetSideName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSideName(", side, ")");
        const char* res = fptr_GetSideName(side);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSideStartUnit(QString __id, int side) {
    if (fptr_GetSideStartUnit == NULL) {
        logger.error("Bad function pointer: GetSideStartUnit");
        throw bad_fptr("GetSideStartUnit");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSideStartUnit(", side, ")");
        const char* res = fptr_GetSideStartUnit(side);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getMapOptionCount(QString __id, QString mapName) {
    if (fptr_GetMapOptionCount == NULL) {
        logger.error("Bad function pointer: GetMapOptionCount");
        throw bad_fptr("GetMapOptionCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetMapOptionCount(", mapName.toStdString().c_str(), ")");
        int res = fptr_GetMapOptionCount(mapName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getModOptionCount(QString __id) {
    if (fptr_GetModOptionCount == NULL) {
        logger.error("Bad function pointer: GetModOptionCount");
        throw bad_fptr("GetModOptionCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetModOptionCount(", ")");
        int res = fptr_GetModOptionCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getCustomOptionCount(QString __id, QString fileName) {
    if (fptr_GetCustomOptionCount == NULL) {
        logger.error("Bad function pointer: GetCustomOptionCount");
        throw bad_fptr("GetCustomOptionCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetCustomOptionCount(", fileName.toStdString().c_str(), ")");
        int res = fptr_GetCustomOptionCount(fileName.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionKey(QString __id, int optIndex) {
    if (fptr_GetOptionKey == NULL) {
        logger.error("Bad function pointer: GetOptionKey");
        throw bad_fptr("GetOptionKey");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionKey(", optIndex, ")");
        const char* res = fptr_GetOptionKey(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionScope(QString __id, int optIndex) {
    if (fptr_GetOptionScope == NULL) {
        logger.error("Bad function pointer: GetOptionScope");
        throw bad_fptr("GetOptionScope");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionScope(", optIndex, ")");
        const char* res = fptr_GetOptionScope(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionName(QString __id, int optIndex) {
    if (fptr_GetOptionName == NULL) {
        logger.error("Bad function pointer: GetOptionName");
        throw bad_fptr("GetOptionName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionName(", optIndex, ")");
        const char* res = fptr_GetOptionName(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionSection(QString __id, int optIndex) {
    if (fptr_GetOptionSection == NULL) {
        logger.error("Bad function pointer: GetOptionSection");
        throw bad_fptr("GetOptionSection");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionSection(", optIndex, ")");
        const char* res = fptr_GetOptionSection(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionStyle(QString __id, int optIndex) {
    if (fptr_GetOptionStyle == NULL) {
        logger.error("Bad function pointer: GetOptionStyle");
        throw bad_fptr("GetOptionStyle");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionStyle(", optIndex, ")");
        const char* res = fptr_GetOptionStyle(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionDesc(QString __id, int optIndex) {
    if (fptr_GetOptionDesc == NULL) {
        logger.error("Bad function pointer: GetOptionDesc");
        throw bad_fptr("GetOptionDesc");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionDesc(", optIndex, ")");
        const char* res = fptr_GetOptionDesc(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionType(QString __id, int optIndex) {
    if (fptr_GetOptionType == NULL) {
        logger.error("Bad function pointer: GetOptionType");
        throw bad_fptr("GetOptionType");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionType(", optIndex, ")");
        int res = fptr_GetOptionType(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionBoolDef(QString __id, int optIndex) {
    if (fptr_GetOptionBoolDef == NULL) {
        logger.error("Bad function pointer: GetOptionBoolDef");
        throw bad_fptr("GetOptionBoolDef");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionBoolDef(", optIndex, ")");
        int res = fptr_GetOptionBoolDef(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionNumberDef(QString __id, int optIndex) {
    if (fptr_GetOptionNumberDef == NULL) {
        logger.error("Bad function pointer: GetOptionNumberDef");
        throw bad_fptr("GetOptionNumberDef");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionNumberDef(", optIndex, ")");
        float res = fptr_GetOptionNumberDef(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionNumberMin(QString __id, int optIndex) {
    if (fptr_GetOptionNumberMin == NULL) {
        logger.error("Bad function pointer: GetOptionNumberMin");
        throw bad_fptr("GetOptionNumberMin");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionNumberMin(", optIndex, ")");
        float res = fptr_GetOptionNumberMin(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionNumberMax(QString __id, int optIndex) {
    if (fptr_GetOptionNumberMax == NULL) {
        logger.error("Bad function pointer: GetOptionNumberMax");
        throw bad_fptr("GetOptionNumberMax");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionNumberMax(", optIndex, ")");
        float res = fptr_GetOptionNumberMax(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionNumberStep(QString __id, int optIndex) {
    if (fptr_GetOptionNumberStep == NULL) {
        logger.error("Bad function pointer: GetOptionNumberStep");
        throw bad_fptr("GetOptionNumberStep");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionNumberStep(", optIndex, ")");
        float res = fptr_GetOptionNumberStep(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionStringDef(QString __id, int optIndex) {
    if (fptr_GetOptionStringDef == NULL) {
        logger.error("Bad function pointer: GetOptionStringDef");
        throw bad_fptr("GetOptionStringDef");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionStringDef(", optIndex, ")");
        const char* res = fptr_GetOptionStringDef(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionStringMaxLen(QString __id, int optIndex) {
    if (fptr_GetOptionStringMaxLen == NULL) {
        logger.error("Bad function pointer: GetOptionStringMaxLen");
        throw bad_fptr("GetOptionStringMaxLen");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionStringMaxLen(", optIndex, ")");
        int res = fptr_GetOptionStringMaxLen(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionListCount(QString __id, int optIndex) {
    if (fptr_GetOptionListCount == NULL) {
        logger.error("Bad function pointer: GetOptionListCount");
        throw bad_fptr("GetOptionListCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionListCount(", optIndex, ")");
        int res = fptr_GetOptionListCount(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionListDef(QString __id, int optIndex) {
    if (fptr_GetOptionListDef == NULL) {
        logger.error("Bad function pointer: GetOptionListDef");
        throw bad_fptr("GetOptionListDef");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionListDef(", optIndex, ")");
        const char* res = fptr_GetOptionListDef(optIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionListItemKey(QString __id, int optIndex, int itemIndex) {
    if (fptr_GetOptionListItemKey == NULL) {
        logger.error("Bad function pointer: GetOptionListItemKey");
        throw bad_fptr("GetOptionListItemKey");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionListItemKey(", optIndex, ", ", itemIndex, ")");
        const char* res = fptr_GetOptionListItemKey(optIndex, itemIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionListItemName(QString __id, int optIndex, int itemIndex) {
    if (fptr_GetOptionListItemName == NULL) {
        logger.error("Bad function pointer: GetOptionListItemName");
        throw bad_fptr("GetOptionListItemName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionListItemName(", optIndex, ", ", itemIndex, ")");
        const char* res = fptr_GetOptionListItemName(optIndex, itemIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getOptionListItemDesc(QString __id, int optIndex, int itemIndex) {
    if (fptr_GetOptionListItemDesc == NULL) {
        logger.error("Bad function pointer: GetOptionListItemDesc");
        throw bad_fptr("GetOptionListItemDesc");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetOptionListItemDesc(", optIndex, ", ", itemIndex, ")");
        const char* res = fptr_GetOptionListItemDesc(optIndex, itemIndex);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getModValidMapCount(QString __id) {
    if (fptr_GetModValidMapCount == NULL) {
        logger.error("Bad function pointer: GetModValidMapCount");
        throw bad_fptr("GetModValidMapCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetModValidMapCount(", ")");
        int res = fptr_GetModValidMapCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getModValidMap(QString __id, int index) {
    if (fptr_GetModValidMap == NULL) {
        logger.error("Bad function pointer: GetModValidMap");
        throw bad_fptr("GetModValidMap");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetModValidMap(", index, ")");
        const char* res = fptr_GetModValidMap(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::openFileVFS(QString __id, QString name) {
    if (fptr_OpenFileVFS == NULL) {
        logger.error("Bad function pointer: OpenFileVFS");
        throw bad_fptr("OpenFileVFS");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call OpenFileVFS(", name.toStdString().c_str(), ")");
        int res = fptr_OpenFileVFS(name.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::closeFileVFS(QString __id, int file) {
    if (fptr_CloseFileVFS == NULL) {
        logger.error("Bad function pointer: CloseFileVFS");
        throw bad_fptr("CloseFileVFS");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call CloseFileVFS(", file, ")");
        fptr_CloseFileVFS(file);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::fileSizeVFS(QString __id, int file) {
    if (fptr_FileSizeVFS == NULL) {
        logger.error("Bad function pointer: FileSizeVFS");
        throw bad_fptr("FileSizeVFS");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call FileSizeVFS(", file, ")");
        int res = fptr_FileSizeVFS(file);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::initFindVFS(QString __id, QString pattern) {
    if (fptr_InitFindVFS == NULL) {
        logger.error("Bad function pointer: InitFindVFS");
        throw bad_fptr("InitFindVFS");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call InitFindVFS(", pattern.toStdString().c_str(), ")");
        int res = fptr_InitFindVFS(pattern.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::initDirListVFS(QString __id, QString path, QString pattern, QString modes) {
    if (fptr_InitDirListVFS == NULL) {
        logger.error("Bad function pointer: InitDirListVFS");
        throw bad_fptr("InitDirListVFS");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call InitDirListVFS(", path.toStdString().c_str(), ", ", pattern.toStdString().c_str(), ", ", modes.toStdString().c_str(), ")");
        int res = fptr_InitDirListVFS(path.toStdString().c_str(), pattern.toStdString().c_str(), modes.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::initSubDirsVFS(QString __id, QString path, QString pattern, QString modes) {
    if (fptr_InitSubDirsVFS == NULL) {
        logger.error("Bad function pointer: InitSubDirsVFS");
        throw bad_fptr("InitSubDirsVFS");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call InitSubDirsVFS(", path.toStdString().c_str(), ", ", pattern.toStdString().c_str(), ", ", modes.toStdString().c_str(), ")");
        int res = fptr_InitSubDirsVFS(path.toStdString().c_str(), pattern.toStdString().c_str(), modes.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::openArchive(QString __id, QString name) {
    if (fptr_OpenArchive == NULL) {
        logger.error("Bad function pointer: OpenArchive");
        throw bad_fptr("OpenArchive");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call OpenArchive(", name.toStdString().c_str(), ")");
        int res = fptr_OpenArchive(name.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::closeArchive(QString __id, int archive) {
    if (fptr_CloseArchive == NULL) {
        logger.error("Bad function pointer: CloseArchive");
        throw bad_fptr("CloseArchive");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call CloseArchive(", archive, ")");
        fptr_CloseArchive(archive);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::openArchiveFile(QString __id, int archive, QString name) {
    if (fptr_OpenArchiveFile == NULL) {
        logger.error("Bad function pointer: OpenArchiveFile");
        throw bad_fptr("OpenArchiveFile");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call OpenArchiveFile(", archive, ", ", name.toStdString().c_str(), ")");
        int res = fptr_OpenArchiveFile(archive, name.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::closeArchiveFile(QString __id, int archive, int file) {
    if (fptr_CloseArchiveFile == NULL) {
        logger.error("Bad function pointer: CloseArchiveFile");
        throw bad_fptr("CloseArchiveFile");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call CloseArchiveFile(", archive, ", ", file, ")");
        fptr_CloseArchiveFile(archive, file);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::sizeArchiveFile(QString __id, int archive, int file) {
    if (fptr_SizeArchiveFile == NULL) {
        logger.error("Bad function pointer: SizeArchiveFile");
        throw bad_fptr("SizeArchiveFile");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call SizeArchiveFile(", archive, ", ", file, ")");
        int res = fptr_SizeArchiveFile(archive, file);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::setSpringConfigFile(QString __id, QString fileNameAsAbsolutePath) {
    if (fptr_SetSpringConfigFile == NULL) {
        logger.error("Bad function pointer: SetSpringConfigFile");
        throw bad_fptr("SetSpringConfigFile");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call SetSpringConfigFile(", fileNameAsAbsolutePath.toStdString().c_str(), ")");
        fptr_SetSpringConfigFile(fileNameAsAbsolutePath.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSpringConfigFile(QString __id) {
    if (fptr_GetSpringConfigFile == NULL) {
        logger.error("Bad function pointer: GetSpringConfigFile");
        throw bad_fptr("GetSpringConfigFile");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSpringConfigFile(", ")");
        const char* res = fptr_GetSpringConfigFile();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSpringConfigString(QString __id, QString name, QString defValue) {
    if (fptr_GetSpringConfigString == NULL) {
        logger.error("Bad function pointer: GetSpringConfigString");
        throw bad_fptr("GetSpringConfigString");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSpringConfigString(", name.toStdString().c_str(), ", ", defValue.toStdString().c_str(), ")");
        const char* res = fptr_GetSpringConfigString(name.toStdString().c_str(), defValue.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSpringConfigInt(QString __id, QString name, int defValue) {
    if (fptr_GetSpringConfigInt == NULL) {
        logger.error("Bad function pointer: GetSpringConfigInt");
        throw bad_fptr("GetSpringConfigInt");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSpringConfigInt(", name.toStdString().c_str(), ", ", defValue, ")");
        int res = fptr_GetSpringConfigInt(name.toStdString().c_str(), defValue);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getSpringConfigFloat(QString __id, QString name, float defValue) {
    if (fptr_GetSpringConfigFloat == NULL) {
        logger.error("Bad function pointer: GetSpringConfigFloat");
        throw bad_fptr("GetSpringConfigFloat");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetSpringConfigFloat(", name.toStdString().c_str(), ", ", defValue, ")");
        float res = fptr_GetSpringConfigFloat(name.toStdString().c_str(), defValue);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::setSpringConfigString(QString __id, QString name, QString value) {
    if (fptr_SetSpringConfigString == NULL) {
        logger.error("Bad function pointer: SetSpringConfigString");
        throw bad_fptr("SetSpringConfigString");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call SetSpringConfigString(", name.toStdString().c_str(), ", ", value.toStdString().c_str(), ")");
        fptr_SetSpringConfigString(name.toStdString().c_str(), value.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::setSpringConfigInt(QString __id, QString name, int value) {
    if (fptr_SetSpringConfigInt == NULL) {
        logger.error("Bad function pointer: SetSpringConfigInt");
        throw bad_fptr("SetSpringConfigInt");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call SetSpringConfigInt(", name.toStdString().c_str(), ", ", value, ")");
        fptr_SetSpringConfigInt(name.toStdString().c_str(), value);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::setSpringConfigFloat(QString __id, QString name, float value) {
    if (fptr_SetSpringConfigFloat == NULL) {
        logger.error("Bad function pointer: SetSpringConfigFloat");
        throw bad_fptr("SetSpringConfigFloat");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call SetSpringConfigFloat(", name.toStdString().c_str(), ", ", value, ")");
        fptr_SetSpringConfigFloat(name.toStdString().c_str(), value);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::deleteSpringConfigKey(QString __id, QString name) {
    if (fptr_DeleteSpringConfigKey == NULL) {
        logger.error("Bad function pointer: DeleteSpringConfigKey");
        throw bad_fptr("DeleteSpringConfigKey");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call DeleteSpringConfigKey(", name.toStdString().c_str(), ")");
        fptr_DeleteSpringConfigKey(name.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpClose(QString __id) {
    if (fptr_lpClose == NULL) {
        logger.error("Bad function pointer: lpClose");
        throw bad_fptr("lpClose");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpClose(", ")");
        fptr_lpClose();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpOpenFile(QString __id, QString fileName, QString fileModes, QString accessModes) {
    if (fptr_lpOpenFile == NULL) {
        logger.error("Bad function pointer: lpOpenFile");
        throw bad_fptr("lpOpenFile");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpOpenFile(", fileName.toStdString().c_str(), ", ", fileModes.toStdString().c_str(), ", ", accessModes.toStdString().c_str(), ")");
        int res = fptr_lpOpenFile(fileName.toStdString().c_str(), fileModes.toStdString().c_str(), accessModes.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpOpenSource(QString __id, QString source, QString accessModes) {
    if (fptr_lpOpenSource == NULL) {
        logger.error("Bad function pointer: lpOpenSource");
        throw bad_fptr("lpOpenSource");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpOpenSource(", source.toStdString().c_str(), ", ", accessModes.toStdString().c_str(), ")");
        int res = fptr_lpOpenSource(source.toStdString().c_str(), accessModes.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpExecute(QString __id) {
    if (fptr_lpExecute == NULL) {
        logger.error("Bad function pointer: lpExecute");
        throw bad_fptr("lpExecute");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpExecute(", ")");
        int res = fptr_lpExecute();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpErrorLog(QString __id) {
    if (fptr_lpErrorLog == NULL) {
        logger.error("Bad function pointer: lpErrorLog");
        throw bad_fptr("lpErrorLog");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpErrorLog(", ")");
        const char* res = fptr_lpErrorLog();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddTableInt(QString __id, int key, int override) {
    if (fptr_lpAddTableInt == NULL) {
        logger.error("Bad function pointer: lpAddTableInt");
        throw bad_fptr("lpAddTableInt");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddTableInt(", key, ", ", override, ")");
        fptr_lpAddTableInt(key, override);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddTableStr(QString __id, QString key, int override) {
    if (fptr_lpAddTableStr == NULL) {
        logger.error("Bad function pointer: lpAddTableStr");
        throw bad_fptr("lpAddTableStr");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddTableStr(", key.toStdString().c_str(), ", ", override, ")");
        fptr_lpAddTableStr(key.toStdString().c_str(), override);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpEndTable(QString __id) {
    if (fptr_lpEndTable == NULL) {
        logger.error("Bad function pointer: lpEndTable");
        throw bad_fptr("lpEndTable");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpEndTable(", ")");
        fptr_lpEndTable();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddIntKeyIntVal(QString __id, int key, int value) {
    if (fptr_lpAddIntKeyIntVal == NULL) {
        logger.error("Bad function pointer: lpAddIntKeyIntVal");
        throw bad_fptr("lpAddIntKeyIntVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddIntKeyIntVal(", key, ", ", value, ")");
        fptr_lpAddIntKeyIntVal(key, value);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddStrKeyIntVal(QString __id, QString key, int value) {
    if (fptr_lpAddStrKeyIntVal == NULL) {
        logger.error("Bad function pointer: lpAddStrKeyIntVal");
        throw bad_fptr("lpAddStrKeyIntVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddStrKeyIntVal(", key.toStdString().c_str(), ", ", value, ")");
        fptr_lpAddStrKeyIntVal(key.toStdString().c_str(), value);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddIntKeyBoolVal(QString __id, int key, int value) {
    if (fptr_lpAddIntKeyBoolVal == NULL) {
        logger.error("Bad function pointer: lpAddIntKeyBoolVal");
        throw bad_fptr("lpAddIntKeyBoolVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddIntKeyBoolVal(", key, ", ", value, ")");
        fptr_lpAddIntKeyBoolVal(key, value);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddStrKeyBoolVal(QString __id, QString key, int value) {
    if (fptr_lpAddStrKeyBoolVal == NULL) {
        logger.error("Bad function pointer: lpAddStrKeyBoolVal");
        throw bad_fptr("lpAddStrKeyBoolVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddStrKeyBoolVal(", key.toStdString().c_str(), ", ", value, ")");
        fptr_lpAddStrKeyBoolVal(key.toStdString().c_str(), value);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddIntKeyFloatVal(QString __id, int key, float value) {
    if (fptr_lpAddIntKeyFloatVal == NULL) {
        logger.error("Bad function pointer: lpAddIntKeyFloatVal");
        throw bad_fptr("lpAddIntKeyFloatVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddIntKeyFloatVal(", key, ", ", value, ")");
        fptr_lpAddIntKeyFloatVal(key, value);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddStrKeyFloatVal(QString __id, QString key, float value) {
    if (fptr_lpAddStrKeyFloatVal == NULL) {
        logger.error("Bad function pointer: lpAddStrKeyFloatVal");
        throw bad_fptr("lpAddStrKeyFloatVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddStrKeyFloatVal(", key.toStdString().c_str(), ", ", value, ")");
        fptr_lpAddStrKeyFloatVal(key.toStdString().c_str(), value);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddIntKeyStrVal(QString __id, int key, QString value) {
    if (fptr_lpAddIntKeyStrVal == NULL) {
        logger.error("Bad function pointer: lpAddIntKeyStrVal");
        throw bad_fptr("lpAddIntKeyStrVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddIntKeyStrVal(", key, ", ", value.toStdString().c_str(), ")");
        fptr_lpAddIntKeyStrVal(key, value.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpAddStrKeyStrVal(QString __id, QString key, QString value) {
    if (fptr_lpAddStrKeyStrVal == NULL) {
        logger.error("Bad function pointer: lpAddStrKeyStrVal");
        throw bad_fptr("lpAddStrKeyStrVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpAddStrKeyStrVal(", key.toStdString().c_str(), ", ", value.toStdString().c_str(), ")");
        fptr_lpAddStrKeyStrVal(key.toStdString().c_str(), value.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpRootTable(QString __id) {
    if (fptr_lpRootTable == NULL) {
        logger.error("Bad function pointer: lpRootTable");
        throw bad_fptr("lpRootTable");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpRootTable(", ")");
        int res = fptr_lpRootTable();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpRootTableExpr(QString __id, QString expr) {
    if (fptr_lpRootTableExpr == NULL) {
        logger.error("Bad function pointer: lpRootTableExpr");
        throw bad_fptr("lpRootTableExpr");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpRootTableExpr(", expr.toStdString().c_str(), ")");
        int res = fptr_lpRootTableExpr(expr.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpSubTableInt(QString __id, int key) {
    if (fptr_lpSubTableInt == NULL) {
        logger.error("Bad function pointer: lpSubTableInt");
        throw bad_fptr("lpSubTableInt");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpSubTableInt(", key, ")");
        int res = fptr_lpSubTableInt(key);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpSubTableStr(QString __id, QString key) {
    if (fptr_lpSubTableStr == NULL) {
        logger.error("Bad function pointer: lpSubTableStr");
        throw bad_fptr("lpSubTableStr");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpSubTableStr(", key.toStdString().c_str(), ")");
        int res = fptr_lpSubTableStr(key.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpSubTableExpr(QString __id, QString expr) {
    if (fptr_lpSubTableExpr == NULL) {
        logger.error("Bad function pointer: lpSubTableExpr");
        throw bad_fptr("lpSubTableExpr");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpSubTableExpr(", expr.toStdString().c_str(), ")");
        int res = fptr_lpSubTableExpr(expr.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpPopTable(QString __id) {
    if (fptr_lpPopTable == NULL) {
        logger.error("Bad function pointer: lpPopTable");
        throw bad_fptr("lpPopTable");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpPopTable(", ")");
        fptr_lpPopTable();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "void", ""));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetKeyExistsInt(QString __id, int key) {
    if (fptr_lpGetKeyExistsInt == NULL) {
        logger.error("Bad function pointer: lpGetKeyExistsInt");
        throw bad_fptr("lpGetKeyExistsInt");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetKeyExistsInt(", key, ")");
        int res = fptr_lpGetKeyExistsInt(key);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetKeyExistsStr(QString __id, QString key) {
    if (fptr_lpGetKeyExistsStr == NULL) {
        logger.error("Bad function pointer: lpGetKeyExistsStr");
        throw bad_fptr("lpGetKeyExistsStr");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetKeyExistsStr(", key.toStdString().c_str(), ")");
        int res = fptr_lpGetKeyExistsStr(key.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetIntKeyType(QString __id, int key) {
    if (fptr_lpGetIntKeyType == NULL) {
        logger.error("Bad function pointer: lpGetIntKeyType");
        throw bad_fptr("lpGetIntKeyType");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetIntKeyType(", key, ")");
        int res = fptr_lpGetIntKeyType(key);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetStrKeyType(QString __id, QString key) {
    if (fptr_lpGetStrKeyType == NULL) {
        logger.error("Bad function pointer: lpGetStrKeyType");
        throw bad_fptr("lpGetStrKeyType");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetStrKeyType(", key.toStdString().c_str(), ")");
        int res = fptr_lpGetStrKeyType(key.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetIntKeyListCount(QString __id) {
    if (fptr_lpGetIntKeyListCount == NULL) {
        logger.error("Bad function pointer: lpGetIntKeyListCount");
        throw bad_fptr("lpGetIntKeyListCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetIntKeyListCount(", ")");
        int res = fptr_lpGetIntKeyListCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetIntKeyListEntry(QString __id, int index) {
    if (fptr_lpGetIntKeyListEntry == NULL) {
        logger.error("Bad function pointer: lpGetIntKeyListEntry");
        throw bad_fptr("lpGetIntKeyListEntry");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetIntKeyListEntry(", index, ")");
        int res = fptr_lpGetIntKeyListEntry(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetStrKeyListCount(QString __id) {
    if (fptr_lpGetStrKeyListCount == NULL) {
        logger.error("Bad function pointer: lpGetStrKeyListCount");
        throw bad_fptr("lpGetStrKeyListCount");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetStrKeyListCount(", ")");
        int res = fptr_lpGetStrKeyListCount();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetStrKeyListEntry(QString __id, int index) {
    if (fptr_lpGetStrKeyListEntry == NULL) {
        logger.error("Bad function pointer: lpGetStrKeyListEntry");
        throw bad_fptr("lpGetStrKeyListEntry");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetStrKeyListEntry(", index, ")");
        const char* res = fptr_lpGetStrKeyListEntry(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetIntKeyIntVal(QString __id, int key, int defValue) {
    if (fptr_lpGetIntKeyIntVal == NULL) {
        logger.error("Bad function pointer: lpGetIntKeyIntVal");
        throw bad_fptr("lpGetIntKeyIntVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetIntKeyIntVal(", key, ", ", defValue, ")");
        int res = fptr_lpGetIntKeyIntVal(key, defValue);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetStrKeyIntVal(QString __id, QString key, int defValue) {
    if (fptr_lpGetStrKeyIntVal == NULL) {
        logger.error("Bad function pointer: lpGetStrKeyIntVal");
        throw bad_fptr("lpGetStrKeyIntVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetStrKeyIntVal(", key.toStdString().c_str(), ", ", defValue, ")");
        int res = fptr_lpGetStrKeyIntVal(key.toStdString().c_str(), defValue);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetIntKeyBoolVal(QString __id, int key, int defValue) {
    if (fptr_lpGetIntKeyBoolVal == NULL) {
        logger.error("Bad function pointer: lpGetIntKeyBoolVal");
        throw bad_fptr("lpGetIntKeyBoolVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetIntKeyBoolVal(", key, ", ", defValue, ")");
        int res = fptr_lpGetIntKeyBoolVal(key, defValue);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetStrKeyBoolVal(QString __id, QString key, int defValue) {
    if (fptr_lpGetStrKeyBoolVal == NULL) {
        logger.error("Bad function pointer: lpGetStrKeyBoolVal");
        throw bad_fptr("lpGetStrKeyBoolVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetStrKeyBoolVal(", key.toStdString().c_str(), ", ", defValue, ")");
        int res = fptr_lpGetStrKeyBoolVal(key.toStdString().c_str(), defValue);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetIntKeyFloatVal(QString __id, int key, float defValue) {
    if (fptr_lpGetIntKeyFloatVal == NULL) {
        logger.error("Bad function pointer: lpGetIntKeyFloatVal");
        throw bad_fptr("lpGetIntKeyFloatVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetIntKeyFloatVal(", key, ", ", defValue, ")");
        float res = fptr_lpGetIntKeyFloatVal(key, defValue);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetStrKeyFloatVal(QString __id, QString key, float defValue) {
    if (fptr_lpGetStrKeyFloatVal == NULL) {
        logger.error("Bad function pointer: lpGetStrKeyFloatVal");
        throw bad_fptr("lpGetStrKeyFloatVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetStrKeyFloatVal(", key.toStdString().c_str(), ", ", defValue, ")");
        float res = fptr_lpGetStrKeyFloatVal(key.toStdString().c_str(), defValue);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "float", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetIntKeyStrVal(QString __id, int key, QString defValue) {
    if (fptr_lpGetIntKeyStrVal == NULL) {
        logger.error("Bad function pointer: lpGetIntKeyStrVal");
        throw bad_fptr("lpGetIntKeyStrVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetIntKeyStrVal(", key, ", ", defValue.toStdString().c_str(), ")");
        const char* res = fptr_lpGetIntKeyStrVal(key, defValue.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::lpGetStrKeyStrVal(QString __id, QString key, QString defValue) {
    if (fptr_lpGetStrKeyStrVal == NULL) {
        logger.error("Bad function pointer: lpGetStrKeyStrVal");
        throw bad_fptr("lpGetStrKeyStrVal");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call lpGetStrKeyStrVal(", key.toStdString().c_str(), ", ", defValue.toStdString().c_str(), ")");
        const char* res = fptr_lpGetStrKeyStrVal(key.toStdString().c_str(), defValue.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::processUnitsNoChecksum(QString __id) {
    if (fptr_ProcessUnitsNoChecksum == NULL) {
        logger.error("Bad function pointer: ProcessUnitsNoChecksum");
        throw bad_fptr("ProcessUnitsNoChecksum");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call ProcessUnitsNoChecksum(", ")");
        int res = fptr_ProcessUnitsNoChecksum();
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getInfoValue(QString __id, int index) {
    if (fptr_GetInfoValue == NULL) {
        logger.error("Bad function pointer: GetInfoValue");
        throw bad_fptr("GetInfoValue");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetInfoValue(", index, ")");
        const char* res = fptr_GetInfoValue(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModName(QString __id, int index) {
    if (fptr_GetPrimaryModName == NULL) {
        logger.error("Bad function pointer: GetPrimaryModName");
        throw bad_fptr("GetPrimaryModName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModName(", index, ")");
        const char* res = fptr_GetPrimaryModName(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModShortName(QString __id, int index) {
    if (fptr_GetPrimaryModShortName == NULL) {
        logger.error("Bad function pointer: GetPrimaryModShortName");
        throw bad_fptr("GetPrimaryModShortName");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModShortName(", index, ")");
        const char* res = fptr_GetPrimaryModShortName(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModVersion(QString __id, int index) {
    if (fptr_GetPrimaryModVersion == NULL) {
        logger.error("Bad function pointer: GetPrimaryModVersion");
        throw bad_fptr("GetPrimaryModVersion");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModVersion(", index, ")");
        const char* res = fptr_GetPrimaryModVersion(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModMutator(QString __id, int index) {
    if (fptr_GetPrimaryModMutator == NULL) {
        logger.error("Bad function pointer: GetPrimaryModMutator");
        throw bad_fptr("GetPrimaryModMutator");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModMutator(", index, ")");
        const char* res = fptr_GetPrimaryModMutator(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModGame(QString __id, int index) {
    if (fptr_GetPrimaryModGame == NULL) {
        logger.error("Bad function pointer: GetPrimaryModGame");
        throw bad_fptr("GetPrimaryModGame");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModGame(", index, ")");
        const char* res = fptr_GetPrimaryModGame(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModShortGame(QString __id, int index) {
    if (fptr_GetPrimaryModShortGame == NULL) {
        logger.error("Bad function pointer: GetPrimaryModShortGame");
        throw bad_fptr("GetPrimaryModShortGame");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModShortGame(", index, ")");
        const char* res = fptr_GetPrimaryModShortGame(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::getPrimaryModDescription(QString __id, int index) {
    if (fptr_GetPrimaryModDescription == NULL) {
        logger.error("Bad function pointer: GetPrimaryModDescription");
        throw bad_fptr("GetPrimaryModDescription");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call GetPrimaryModDescription(", index, ")");
        const char* res = fptr_GetPrimaryModDescription(index);
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "const char*", res));
    });
    queueCond.notify_all();
}
void UnitsyncHandlerAsync::openArchiveType(QString __id, QString name, QString type) {
    if (fptr_OpenArchiveType == NULL) {
        logger.error("Bad function pointer: OpenArchiveType");
        throw bad_fptr("OpenArchiveType");
    }
    boost::unique_lock<boost::mutex> lock(queueMutex);
    queue.push([=](){
        logger.debug("call OpenArchiveType(", name.toStdString().c_str(), ", ", type.toStdString().c_str(), ")");
        int res = fptr_OpenArchiveType(name.toStdString().c_str(), type.toStdString().c_str());
        QCoreApplication::postEvent(parent(), new ResultEvent(__id.toStdString(), "int", std::to_string(res)));
    });
    queueCond.notify_all();
}
