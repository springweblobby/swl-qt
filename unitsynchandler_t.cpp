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
                    func = queue.front();
                    queue.pop();
                }{
                    boost::lock_guard<boost::mutex> lock(executionMutex);
                    func();
                }
            }
        });
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
QString UnitsyncHandlerAsync::jsReadFileVFS(int fd, int size) {
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
    QString res;
    char tmp[8];
    for (int i = 0; i < size; i++) {
        std::snprintf(tmp, 8, "%%%.2hhX", readBuf[i + off]);
        res += QString(tmp);
    }
    return res;
}

QString UnitsyncHandlerAsync::init(bool isServer, int id) {
    if (fptr_Init == NULL) {
        logger.error("Bad function pointer: Init");
        throw bad_fptr("Init");
    }
    logger.debug("call Init(", isServer, ", ", id, ")");
    //return fptr_Init(isServer, id);
    return "asdf";
}
