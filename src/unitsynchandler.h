// DO NOT EDIT: THIS FILE WAS GENERATED by unitsync wrapper generator
// from unitsynchandler.h.template. Edit that file instead.
#ifndef _UNITSYNC_HANDLER_H
#define _UNITSYNC_HANDLER_H

#include "logger.h"
#include <string>
#include <exception>
#include <boost/filesystem.hpp>
#include <QObject>

class MapInfo;
class UnitsyncHandler : public QObject {
    Q_OBJECT
public:
    UnitsyncHandler(QObject* parent, Logger&, boost::filesystem::path dllpath);
    ~UnitsyncHandler();
    bool isReady() {
        return ready;
    }

    UnitsyncHandler(const UnitsyncHandler&) = delete;
    UnitsyncHandler& operator=(const UnitsyncHandler&) = delete;
    UnitsyncHandler(UnitsyncHandler&&);

    struct bad_fptr : public std::exception {
        bad_fptr(std::string symbol) {
            m_what = "Symbol " + symbol + " not found in the unitsync library.";
        }
        const char* what() { return m_what.c_str(); }
        std::string m_what;
    };

public slots:

    QString jsReadFileVFS(int fd, int size);

    // Unitsync functions.

    QString getNextError();
    QString getSpringVersion();
    QString getSpringVersionPatchset();
    bool isSpringReleaseVersion();
    int init(bool, int);
    void unInit();
    QString getWritableDataDirectory();
    int getDataDirectoryCount();
    QString getDataDirectory(int);
    int processUnits();
    int getUnitCount();
    QString getUnitName(int);
    QString getFullUnitName(int);
    void addArchive(QString);
    void addAllArchives(QString);
    void removeAllArchives();
    int getArchiveChecksum(QString);
    QString getArchivePath(QString);
    int getMapCount();
    QString getMapName(int);
    QString getMapFileName(int);
    QString getMapDescription(int);
    QString getMapAuthor(int);
    int getMapWidth(int);
    int getMapHeight(int);
    int getMapTidalStrength(int);
    int getMapWindMin(int);
    int getMapWindMax(int);
    int getMapGravity(int);
    int getMapResourceCount(int);
    QString getMapResourceName(int, int);
    float getMapResourceMax(int, int);
    int getMapResourceExtractorRadius(int, int);
    int getMapPosCount(int);
    float getMapPosX(int, int);
    float getMapPosZ(int, int);
    float getMapMinHeight(QString);
    float getMapMaxHeight(QString);
    int getMapArchiveCount(QString);
    QString getMapArchiveName(int);
    int getMapChecksum(int);
    int getMapChecksumFromName(QString);
    int getSkirmishAICount();
    int getSkirmishAIInfoCount(int);
    QString getInfoKey(int);
    QString getInfoType(int);
    QString getInfoValueString(int);
    int getInfoValueInteger(int);
    float getInfoValueFloat(int);
    bool getInfoValueBool(int);
    QString getInfoDescription(int);
    int getSkirmishAIOptionCount(int);
    int getPrimaryModCount();
    int getPrimaryModInfoCount(int);
    QString getPrimaryModArchive(int);
    int getPrimaryModArchiveCount(int);
    QString getPrimaryModArchiveList(int);
    int getPrimaryModIndex(QString);
    int getPrimaryModChecksum(int);
    int getPrimaryModChecksumFromName(QString);
    int getSideCount();
    QString getSideName(int);
    QString getSideStartUnit(int);
    int getMapOptionCount(QString);
    int getModOptionCount();
    int getCustomOptionCount(QString);
    QString getOptionKey(int);
    QString getOptionScope(int);
    QString getOptionName(int);
    QString getOptionSection(int);
    QString getOptionStyle(int);
    QString getOptionDesc(int);
    int getOptionType(int);
    int getOptionBoolDef(int);
    float getOptionNumberDef(int);
    float getOptionNumberMin(int);
    float getOptionNumberMax(int);
    float getOptionNumberStep(int);
    QString getOptionStringDef(int);
    int getOptionStringMaxLen(int);
    int getOptionListCount(int);
    QString getOptionListDef(int);
    QString getOptionListItemKey(int, int);
    QString getOptionListItemName(int, int);
    QString getOptionListItemDesc(int, int);
    int getModValidMapCount();
    QString getModValidMap(int);
    int openFileVFS(QString);
    void closeFileVFS(int);
    int fileSizeVFS(int);
    int initFindVFS(QString);
    int initDirListVFS(QString, QString, QString);
    int initSubDirsVFS(QString, QString, QString);
    int openArchive(QString);
    void closeArchive(int);
    int openArchiveFile(int, QString);
    void closeArchiveFile(int, int);
    int sizeArchiveFile(int, int);
    void setSpringConfigFile(QString);
    QString getSpringConfigFile();
    QString getSpringConfigString(QString, QString);
    int getSpringConfigInt(QString, int);
    float getSpringConfigFloat(QString, float);
    void setSpringConfigString(QString, QString);
    void setSpringConfigInt(QString, int);
    void setSpringConfigFloat(QString, float);
    void deleteSpringConfigKey(QString);
    void lpClose();
    int lpOpenFile(QString, QString, QString);
    int lpOpenSource(QString, QString);
    int lpExecute();
    QString lpErrorLog();
    void lpAddTableInt(int, int);
    void lpAddTableStr(QString, int);
    void lpEndTable();
    void lpAddIntKeyIntVal(int, int);
    void lpAddStrKeyIntVal(QString, int);
    void lpAddIntKeyBoolVal(int, int);
    void lpAddStrKeyBoolVal(QString, int);
    void lpAddIntKeyFloatVal(int, float);
    void lpAddStrKeyFloatVal(QString, float);
    void lpAddIntKeyStrVal(int, QString);
    void lpAddStrKeyStrVal(QString, QString);
    int lpRootTable();
    int lpRootTableExpr(QString);
    int lpSubTableInt(int);
    int lpSubTableStr(QString);
    int lpSubTableExpr(QString);
    void lpPopTable();
    int lpGetKeyExistsInt(int);
    int lpGetKeyExistsStr(QString);
    int lpGetIntKeyType(int);
    int lpGetStrKeyType(QString);
    int lpGetIntKeyListCount();
    int lpGetIntKeyListEntry(int);
    int lpGetStrKeyListCount();
    QString lpGetStrKeyListEntry(int);
    int lpGetIntKeyIntVal(int, int);
    int lpGetStrKeyIntVal(QString, int);
    int lpGetIntKeyBoolVal(int, int);
    int lpGetStrKeyBoolVal(QString, int);
    float lpGetIntKeyFloatVal(int, float);
    float lpGetStrKeyFloatVal(QString, float);
    QString lpGetIntKeyStrVal(int, QString);
    QString lpGetStrKeyStrVal(QString, QString);
    int processUnitsNoChecksum();
    QString getInfoValue(int);
    QString getPrimaryModName(int);
    QString getPrimaryModShortName(int);
    QString getPrimaryModVersion(int);
    QString getPrimaryModMutator(int);
    QString getPrimaryModGame(int);
    QString getPrimaryModShortGame(int);
    QString getPrimaryModDescription(int);
    int openArchiveType(QString, QString);

private:
    Logger& logger;
    bool ready;
    void* handle;

    // Unisync function pointers.

    typedef const char* (*fptr_type_GetNextError)();
    fptr_type_GetNextError fptr_GetNextError;
    typedef const char* (*fptr_type_GetSpringVersion)();
    fptr_type_GetSpringVersion fptr_GetSpringVersion;
    typedef const char* (*fptr_type_GetSpringVersionPatchset)();
    fptr_type_GetSpringVersionPatchset fptr_GetSpringVersionPatchset;
    typedef bool (*fptr_type_IsSpringReleaseVersion)();
    fptr_type_IsSpringReleaseVersion fptr_IsSpringReleaseVersion;
    typedef int (*fptr_type_Init)(bool, int);
    fptr_type_Init fptr_Init;
    typedef void (*fptr_type_UnInit)();
    fptr_type_UnInit fptr_UnInit;
    typedef const char* (*fptr_type_GetWritableDataDirectory)();
    fptr_type_GetWritableDataDirectory fptr_GetWritableDataDirectory;
    typedef int (*fptr_type_GetDataDirectoryCount)();
    fptr_type_GetDataDirectoryCount fptr_GetDataDirectoryCount;
    typedef const char* (*fptr_type_GetDataDirectory)(int);
    fptr_type_GetDataDirectory fptr_GetDataDirectory;
    typedef int (*fptr_type_ProcessUnits)();
    fptr_type_ProcessUnits fptr_ProcessUnits;
    typedef int (*fptr_type_GetUnitCount)();
    fptr_type_GetUnitCount fptr_GetUnitCount;
    typedef const char* (*fptr_type_GetUnitName)(int);
    fptr_type_GetUnitName fptr_GetUnitName;
    typedef const char* (*fptr_type_GetFullUnitName)(int);
    fptr_type_GetFullUnitName fptr_GetFullUnitName;
    typedef void (*fptr_type_AddArchive)(const char*);
    fptr_type_AddArchive fptr_AddArchive;
    typedef void (*fptr_type_AddAllArchives)(const char*);
    fptr_type_AddAllArchives fptr_AddAllArchives;
    typedef void (*fptr_type_RemoveAllArchives)();
    fptr_type_RemoveAllArchives fptr_RemoveAllArchives;
    typedef unsigned int (*fptr_type_GetArchiveChecksum)(const char*);
    fptr_type_GetArchiveChecksum fptr_GetArchiveChecksum;
    typedef const char* (*fptr_type_GetArchivePath)(const char*);
    fptr_type_GetArchivePath fptr_GetArchivePath;
    typedef int (*fptr_type_GetMapCount)();
    fptr_type_GetMapCount fptr_GetMapCount;
    typedef const char* (*fptr_type_GetMapName)(int);
    fptr_type_GetMapName fptr_GetMapName;
    typedef const char* (*fptr_type_GetMapFileName)(int);
    fptr_type_GetMapFileName fptr_GetMapFileName;
    typedef const char* (*fptr_type_GetMapDescription)(int);
    fptr_type_GetMapDescription fptr_GetMapDescription;
    typedef const char* (*fptr_type_GetMapAuthor)(int);
    fptr_type_GetMapAuthor fptr_GetMapAuthor;
    typedef int (*fptr_type_GetMapWidth)(int);
    fptr_type_GetMapWidth fptr_GetMapWidth;
    typedef int (*fptr_type_GetMapHeight)(int);
    fptr_type_GetMapHeight fptr_GetMapHeight;
    typedef int (*fptr_type_GetMapTidalStrength)(int);
    fptr_type_GetMapTidalStrength fptr_GetMapTidalStrength;
    typedef int (*fptr_type_GetMapWindMin)(int);
    fptr_type_GetMapWindMin fptr_GetMapWindMin;
    typedef int (*fptr_type_GetMapWindMax)(int);
    fptr_type_GetMapWindMax fptr_GetMapWindMax;
    typedef int (*fptr_type_GetMapGravity)(int);
    fptr_type_GetMapGravity fptr_GetMapGravity;
    typedef int (*fptr_type_GetMapResourceCount)(int);
    fptr_type_GetMapResourceCount fptr_GetMapResourceCount;
    typedef const char* (*fptr_type_GetMapResourceName)(int, int);
    fptr_type_GetMapResourceName fptr_GetMapResourceName;
    typedef float (*fptr_type_GetMapResourceMax)(int, int);
    fptr_type_GetMapResourceMax fptr_GetMapResourceMax;
    typedef int (*fptr_type_GetMapResourceExtractorRadius)(int, int);
    fptr_type_GetMapResourceExtractorRadius fptr_GetMapResourceExtractorRadius;
    typedef int (*fptr_type_GetMapPosCount)(int);
    fptr_type_GetMapPosCount fptr_GetMapPosCount;
    typedef float (*fptr_type_GetMapPosX)(int, int);
    fptr_type_GetMapPosX fptr_GetMapPosX;
    typedef float (*fptr_type_GetMapPosZ)(int, int);
    fptr_type_GetMapPosZ fptr_GetMapPosZ;
    typedef float (*fptr_type_GetMapMinHeight)(const char*);
    fptr_type_GetMapMinHeight fptr_GetMapMinHeight;
    typedef float (*fptr_type_GetMapMaxHeight)(const char*);
    fptr_type_GetMapMaxHeight fptr_GetMapMaxHeight;
    typedef int (*fptr_type_GetMapArchiveCount)(const char*);
    fptr_type_GetMapArchiveCount fptr_GetMapArchiveCount;
    typedef const char* (*fptr_type_GetMapArchiveName)(int);
    fptr_type_GetMapArchiveName fptr_GetMapArchiveName;
    typedef unsigned int (*fptr_type_GetMapChecksum)(int);
    fptr_type_GetMapChecksum fptr_GetMapChecksum;
    typedef unsigned int (*fptr_type_GetMapChecksumFromName)(const char*);
    fptr_type_GetMapChecksumFromName fptr_GetMapChecksumFromName;
    typedef unsigned short* (*fptr_type_GetMinimap)(const char*, int);
    fptr_type_GetMinimap fptr_GetMinimap;
    typedef int (*fptr_type_GetInfoMapSize)(const char*, const char*, int*, int*);
    fptr_type_GetInfoMapSize fptr_GetInfoMapSize;
    typedef int (*fptr_type_GetInfoMap)(const char*, const char*, unsigned char*, int);
    fptr_type_GetInfoMap fptr_GetInfoMap;
    typedef int (*fptr_type_GetSkirmishAICount)();
    fptr_type_GetSkirmishAICount fptr_GetSkirmishAICount;
    typedef int (*fptr_type_GetSkirmishAIInfoCount)(int);
    fptr_type_GetSkirmishAIInfoCount fptr_GetSkirmishAIInfoCount;
    typedef const char* (*fptr_type_GetInfoKey)(int);
    fptr_type_GetInfoKey fptr_GetInfoKey;
    typedef const char* (*fptr_type_GetInfoType)(int);
    fptr_type_GetInfoType fptr_GetInfoType;
    typedef const char* (*fptr_type_GetInfoValueString)(int);
    fptr_type_GetInfoValueString fptr_GetInfoValueString;
    typedef int (*fptr_type_GetInfoValueInteger)(int);
    fptr_type_GetInfoValueInteger fptr_GetInfoValueInteger;
    typedef float (*fptr_type_GetInfoValueFloat)(int);
    fptr_type_GetInfoValueFloat fptr_GetInfoValueFloat;
    typedef bool (*fptr_type_GetInfoValueBool)(int);
    fptr_type_GetInfoValueBool fptr_GetInfoValueBool;
    typedef const char* (*fptr_type_GetInfoDescription)(int);
    fptr_type_GetInfoDescription fptr_GetInfoDescription;
    typedef int (*fptr_type_GetSkirmishAIOptionCount)(int);
    fptr_type_GetSkirmishAIOptionCount fptr_GetSkirmishAIOptionCount;
    typedef int (*fptr_type_GetPrimaryModCount)();
    fptr_type_GetPrimaryModCount fptr_GetPrimaryModCount;
    typedef int (*fptr_type_GetPrimaryModInfoCount)(int);
    fptr_type_GetPrimaryModInfoCount fptr_GetPrimaryModInfoCount;
    typedef const char* (*fptr_type_GetPrimaryModArchive)(int);
    fptr_type_GetPrimaryModArchive fptr_GetPrimaryModArchive;
    typedef int (*fptr_type_GetPrimaryModArchiveCount)(int);
    fptr_type_GetPrimaryModArchiveCount fptr_GetPrimaryModArchiveCount;
    typedef const char* (*fptr_type_GetPrimaryModArchiveList)(int);
    fptr_type_GetPrimaryModArchiveList fptr_GetPrimaryModArchiveList;
    typedef int (*fptr_type_GetPrimaryModIndex)(const char*);
    fptr_type_GetPrimaryModIndex fptr_GetPrimaryModIndex;
    typedef unsigned int (*fptr_type_GetPrimaryModChecksum)(int);
    fptr_type_GetPrimaryModChecksum fptr_GetPrimaryModChecksum;
    typedef unsigned int (*fptr_type_GetPrimaryModChecksumFromName)(const char*);
    fptr_type_GetPrimaryModChecksumFromName fptr_GetPrimaryModChecksumFromName;
    typedef int (*fptr_type_GetSideCount)();
    fptr_type_GetSideCount fptr_GetSideCount;
    typedef const char* (*fptr_type_GetSideName)(int);
    fptr_type_GetSideName fptr_GetSideName;
    typedef const char* (*fptr_type_GetSideStartUnit)(int);
    fptr_type_GetSideStartUnit fptr_GetSideStartUnit;
    typedef int (*fptr_type_GetMapOptionCount)(const char*);
    fptr_type_GetMapOptionCount fptr_GetMapOptionCount;
    typedef int (*fptr_type_GetModOptionCount)();
    fptr_type_GetModOptionCount fptr_GetModOptionCount;
    typedef int (*fptr_type_GetCustomOptionCount)(const char*);
    fptr_type_GetCustomOptionCount fptr_GetCustomOptionCount;
    typedef const char* (*fptr_type_GetOptionKey)(int);
    fptr_type_GetOptionKey fptr_GetOptionKey;
    typedef const char* (*fptr_type_GetOptionScope)(int);
    fptr_type_GetOptionScope fptr_GetOptionScope;
    typedef const char* (*fptr_type_GetOptionName)(int);
    fptr_type_GetOptionName fptr_GetOptionName;
    typedef const char* (*fptr_type_GetOptionSection)(int);
    fptr_type_GetOptionSection fptr_GetOptionSection;
    typedef const char* (*fptr_type_GetOptionStyle)(int);
    fptr_type_GetOptionStyle fptr_GetOptionStyle;
    typedef const char* (*fptr_type_GetOptionDesc)(int);
    fptr_type_GetOptionDesc fptr_GetOptionDesc;
    typedef int (*fptr_type_GetOptionType)(int);
    fptr_type_GetOptionType fptr_GetOptionType;
    typedef int (*fptr_type_GetOptionBoolDef)(int);
    fptr_type_GetOptionBoolDef fptr_GetOptionBoolDef;
    typedef float (*fptr_type_GetOptionNumberDef)(int);
    fptr_type_GetOptionNumberDef fptr_GetOptionNumberDef;
    typedef float (*fptr_type_GetOptionNumberMin)(int);
    fptr_type_GetOptionNumberMin fptr_GetOptionNumberMin;
    typedef float (*fptr_type_GetOptionNumberMax)(int);
    fptr_type_GetOptionNumberMax fptr_GetOptionNumberMax;
    typedef float (*fptr_type_GetOptionNumberStep)(int);
    fptr_type_GetOptionNumberStep fptr_GetOptionNumberStep;
    typedef const char* (*fptr_type_GetOptionStringDef)(int);
    fptr_type_GetOptionStringDef fptr_GetOptionStringDef;
    typedef int (*fptr_type_GetOptionStringMaxLen)(int);
    fptr_type_GetOptionStringMaxLen fptr_GetOptionStringMaxLen;
    typedef int (*fptr_type_GetOptionListCount)(int);
    fptr_type_GetOptionListCount fptr_GetOptionListCount;
    typedef const char* (*fptr_type_GetOptionListDef)(int);
    fptr_type_GetOptionListDef fptr_GetOptionListDef;
    typedef const char* (*fptr_type_GetOptionListItemKey)(int, int);
    fptr_type_GetOptionListItemKey fptr_GetOptionListItemKey;
    typedef const char* (*fptr_type_GetOptionListItemName)(int, int);
    fptr_type_GetOptionListItemName fptr_GetOptionListItemName;
    typedef const char* (*fptr_type_GetOptionListItemDesc)(int, int);
    fptr_type_GetOptionListItemDesc fptr_GetOptionListItemDesc;
    typedef int (*fptr_type_GetModValidMapCount)();
    fptr_type_GetModValidMapCount fptr_GetModValidMapCount;
    typedef const char* (*fptr_type_GetModValidMap)(int);
    fptr_type_GetModValidMap fptr_GetModValidMap;
    typedef int (*fptr_type_OpenFileVFS)(const char*);
    fptr_type_OpenFileVFS fptr_OpenFileVFS;
    typedef void (*fptr_type_CloseFileVFS)(int);
    fptr_type_CloseFileVFS fptr_CloseFileVFS;
    typedef int (*fptr_type_ReadFileVFS)(int, unsigned char*, int);
    fptr_type_ReadFileVFS fptr_ReadFileVFS;
    typedef int (*fptr_type_FileSizeVFS)(int);
    fptr_type_FileSizeVFS fptr_FileSizeVFS;
    typedef int (*fptr_type_InitFindVFS)(const char*);
    fptr_type_InitFindVFS fptr_InitFindVFS;
    typedef int (*fptr_type_InitDirListVFS)(const char*, const char*, const char*);
    fptr_type_InitDirListVFS fptr_InitDirListVFS;
    typedef int (*fptr_type_InitSubDirsVFS)(const char*, const char*, const char*);
    fptr_type_InitSubDirsVFS fptr_InitSubDirsVFS;
    typedef int (*fptr_type_FindFilesVFS)(int, char*, int);
    fptr_type_FindFilesVFS fptr_FindFilesVFS;
    typedef int (*fptr_type_OpenArchive)(const char*);
    fptr_type_OpenArchive fptr_OpenArchive;
    typedef void (*fptr_type_CloseArchive)(int);
    fptr_type_CloseArchive fptr_CloseArchive;
    typedef int (*fptr_type_FindFilesArchive)(int, int, char*, int*);
    fptr_type_FindFilesArchive fptr_FindFilesArchive;
    typedef int (*fptr_type_OpenArchiveFile)(int, const char*);
    fptr_type_OpenArchiveFile fptr_OpenArchiveFile;
    typedef int (*fptr_type_ReadArchiveFile)(int, int, unsigned char*, int);
    fptr_type_ReadArchiveFile fptr_ReadArchiveFile;
    typedef void (*fptr_type_CloseArchiveFile)(int, int);
    fptr_type_CloseArchiveFile fptr_CloseArchiveFile;
    typedef int (*fptr_type_SizeArchiveFile)(int, int);
    fptr_type_SizeArchiveFile fptr_SizeArchiveFile;
    typedef void (*fptr_type_SetSpringConfigFile)(const char*);
    fptr_type_SetSpringConfigFile fptr_SetSpringConfigFile;
    typedef const char* (*fptr_type_GetSpringConfigFile)();
    fptr_type_GetSpringConfigFile fptr_GetSpringConfigFile;
    typedef const char* (*fptr_type_GetSpringConfigString)(const char*, const char*);
    fptr_type_GetSpringConfigString fptr_GetSpringConfigString;
    typedef int (*fptr_type_GetSpringConfigInt)(const char*, int);
    fptr_type_GetSpringConfigInt fptr_GetSpringConfigInt;
    typedef float (*fptr_type_GetSpringConfigFloat)(const char*, float);
    fptr_type_GetSpringConfigFloat fptr_GetSpringConfigFloat;
    typedef void (*fptr_type_SetSpringConfigString)(const char*, const char*);
    fptr_type_SetSpringConfigString fptr_SetSpringConfigString;
    typedef void (*fptr_type_SetSpringConfigInt)(const char*, int);
    fptr_type_SetSpringConfigInt fptr_SetSpringConfigInt;
    typedef void (*fptr_type_SetSpringConfigFloat)(const char*, float);
    fptr_type_SetSpringConfigFloat fptr_SetSpringConfigFloat;
    typedef void (*fptr_type_DeleteSpringConfigKey)(const char*);
    fptr_type_DeleteSpringConfigKey fptr_DeleteSpringConfigKey;
    typedef void (*fptr_type_lpClose)();
    fptr_type_lpClose fptr_lpClose;
    typedef int (*fptr_type_lpOpenFile)(const char*, const char*, const char*);
    fptr_type_lpOpenFile fptr_lpOpenFile;
    typedef int (*fptr_type_lpOpenSource)(const char*, const char*);
    fptr_type_lpOpenSource fptr_lpOpenSource;
    typedef int (*fptr_type_lpExecute)();
    fptr_type_lpExecute fptr_lpExecute;
    typedef const char* (*fptr_type_lpErrorLog)();
    fptr_type_lpErrorLog fptr_lpErrorLog;
    typedef void (*fptr_type_lpAddTableInt)(int, int);
    fptr_type_lpAddTableInt fptr_lpAddTableInt;
    typedef void (*fptr_type_lpAddTableStr)(const char*, int);
    fptr_type_lpAddTableStr fptr_lpAddTableStr;
    typedef void (*fptr_type_lpEndTable)();
    fptr_type_lpEndTable fptr_lpEndTable;
    typedef void (*fptr_type_lpAddIntKeyIntVal)(int, int);
    fptr_type_lpAddIntKeyIntVal fptr_lpAddIntKeyIntVal;
    typedef void (*fptr_type_lpAddStrKeyIntVal)(const char*, int);
    fptr_type_lpAddStrKeyIntVal fptr_lpAddStrKeyIntVal;
    typedef void (*fptr_type_lpAddIntKeyBoolVal)(int, int);
    fptr_type_lpAddIntKeyBoolVal fptr_lpAddIntKeyBoolVal;
    typedef void (*fptr_type_lpAddStrKeyBoolVal)(const char*, int);
    fptr_type_lpAddStrKeyBoolVal fptr_lpAddStrKeyBoolVal;
    typedef void (*fptr_type_lpAddIntKeyFloatVal)(int, float);
    fptr_type_lpAddIntKeyFloatVal fptr_lpAddIntKeyFloatVal;
    typedef void (*fptr_type_lpAddStrKeyFloatVal)(const char*, float);
    fptr_type_lpAddStrKeyFloatVal fptr_lpAddStrKeyFloatVal;
    typedef void (*fptr_type_lpAddIntKeyStrVal)(int, const char*);
    fptr_type_lpAddIntKeyStrVal fptr_lpAddIntKeyStrVal;
    typedef void (*fptr_type_lpAddStrKeyStrVal)(const char*, const char*);
    fptr_type_lpAddStrKeyStrVal fptr_lpAddStrKeyStrVal;
    typedef int (*fptr_type_lpRootTable)();
    fptr_type_lpRootTable fptr_lpRootTable;
    typedef int (*fptr_type_lpRootTableExpr)(const char*);
    fptr_type_lpRootTableExpr fptr_lpRootTableExpr;
    typedef int (*fptr_type_lpSubTableInt)(int);
    fptr_type_lpSubTableInt fptr_lpSubTableInt;
    typedef int (*fptr_type_lpSubTableStr)(const char*);
    fptr_type_lpSubTableStr fptr_lpSubTableStr;
    typedef int (*fptr_type_lpSubTableExpr)(const char*);
    fptr_type_lpSubTableExpr fptr_lpSubTableExpr;
    typedef void (*fptr_type_lpPopTable)();
    fptr_type_lpPopTable fptr_lpPopTable;
    typedef int (*fptr_type_lpGetKeyExistsInt)(int);
    fptr_type_lpGetKeyExistsInt fptr_lpGetKeyExistsInt;
    typedef int (*fptr_type_lpGetKeyExistsStr)(const char*);
    fptr_type_lpGetKeyExistsStr fptr_lpGetKeyExistsStr;
    typedef int (*fptr_type_lpGetIntKeyType)(int);
    fptr_type_lpGetIntKeyType fptr_lpGetIntKeyType;
    typedef int (*fptr_type_lpGetStrKeyType)(const char*);
    fptr_type_lpGetStrKeyType fptr_lpGetStrKeyType;
    typedef int (*fptr_type_lpGetIntKeyListCount)();
    fptr_type_lpGetIntKeyListCount fptr_lpGetIntKeyListCount;
    typedef int (*fptr_type_lpGetIntKeyListEntry)(int);
    fptr_type_lpGetIntKeyListEntry fptr_lpGetIntKeyListEntry;
    typedef int (*fptr_type_lpGetStrKeyListCount)();
    fptr_type_lpGetStrKeyListCount fptr_lpGetStrKeyListCount;
    typedef const char* (*fptr_type_lpGetStrKeyListEntry)(int);
    fptr_type_lpGetStrKeyListEntry fptr_lpGetStrKeyListEntry;
    typedef int (*fptr_type_lpGetIntKeyIntVal)(int, int);
    fptr_type_lpGetIntKeyIntVal fptr_lpGetIntKeyIntVal;
    typedef int (*fptr_type_lpGetStrKeyIntVal)(const char*, int);
    fptr_type_lpGetStrKeyIntVal fptr_lpGetStrKeyIntVal;
    typedef int (*fptr_type_lpGetIntKeyBoolVal)(int, int);
    fptr_type_lpGetIntKeyBoolVal fptr_lpGetIntKeyBoolVal;
    typedef int (*fptr_type_lpGetStrKeyBoolVal)(const char*, int);
    fptr_type_lpGetStrKeyBoolVal fptr_lpGetStrKeyBoolVal;
    typedef float (*fptr_type_lpGetIntKeyFloatVal)(int, float);
    fptr_type_lpGetIntKeyFloatVal fptr_lpGetIntKeyFloatVal;
    typedef float (*fptr_type_lpGetStrKeyFloatVal)(const char*, float);
    fptr_type_lpGetStrKeyFloatVal fptr_lpGetStrKeyFloatVal;
    typedef const char* (*fptr_type_lpGetIntKeyStrVal)(int, const char*);
    fptr_type_lpGetIntKeyStrVal fptr_lpGetIntKeyStrVal;
    typedef const char* (*fptr_type_lpGetStrKeyStrVal)(const char*, const char*);
    fptr_type_lpGetStrKeyStrVal fptr_lpGetStrKeyStrVal;
    typedef int (*fptr_type_ProcessUnitsNoChecksum)();
    fptr_type_ProcessUnitsNoChecksum fptr_ProcessUnitsNoChecksum;
    typedef int (*fptr_type_GetMapInfoEx)(const char*, MapInfo*, int);
    fptr_type_GetMapInfoEx fptr_GetMapInfoEx;
    typedef int (*fptr_type_GetMapInfo)(const char*, MapInfo*);
    fptr_type_GetMapInfo fptr_GetMapInfo;
    typedef const char* (*fptr_type_GetInfoValue)(int);
    fptr_type_GetInfoValue fptr_GetInfoValue;
    typedef const char* (*fptr_type_GetPrimaryModName)(int);
    fptr_type_GetPrimaryModName fptr_GetPrimaryModName;
    typedef const char* (*fptr_type_GetPrimaryModShortName)(int);
    fptr_type_GetPrimaryModShortName fptr_GetPrimaryModShortName;
    typedef const char* (*fptr_type_GetPrimaryModVersion)(int);
    fptr_type_GetPrimaryModVersion fptr_GetPrimaryModVersion;
    typedef const char* (*fptr_type_GetPrimaryModMutator)(int);
    fptr_type_GetPrimaryModMutator fptr_GetPrimaryModMutator;
    typedef const char* (*fptr_type_GetPrimaryModGame)(int);
    fptr_type_GetPrimaryModGame fptr_GetPrimaryModGame;
    typedef const char* (*fptr_type_GetPrimaryModShortGame)(int);
    fptr_type_GetPrimaryModShortGame fptr_GetPrimaryModShortGame;
    typedef const char* (*fptr_type_GetPrimaryModDescription)(int);
    fptr_type_GetPrimaryModDescription fptr_GetPrimaryModDescription;
    typedef int (*fptr_type_OpenArchiveType)(const char*, const char*);
    fptr_type_OpenArchiveType fptr_OpenArchiveType;
};

#endif // _UNITSYNC_HANDLER_H
