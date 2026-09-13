#pragma once

#include "IComponentInterface.h"
#include "ZCamera.h"
#include "ZStringBuilder.h"
#include "TMap.h"
#include "TArray.h"
#include "ZLevelManager.h"

#include <Common.h>
#include <Functions.h>

class IModule;
class ZString;
class IIniFile;
enum ERunMode;
class IIniFileSection;
class ZIniFileSection;
template<typename T> class TEnumerator;
class ZString;

class ZPath
{
  public:
    ZPath(const ZString& p_Other) : m_value(p_Other) {}

    ZString ToString() const
    {
        return m_value.ToString();
    }

    ZStringBuilder m_value;
};

class ZFilePath
{
  public:
    ZFilePath(const ZString& p_Other) : m_path(p_Other) {}

    ZString ToString() const
    {
        return m_path.ToString();
    }

    ZPath m_path;
};

class IApplication
{
  public:
    virtual ~IApplication() = default;
    virtual void OnBeforeInitialize() = 0;
    virtual void OnAfterInitialize() = 0;
    virtual void OnBeforeShutdown() = 0;
    virtual void OnAfterShutdown() = 0;
    virtual void OnBeforeModuleInitialize(IModule* module) = 0;
    virtual void OnAfterModuleInitialize(IModule* module) = 0;
    virtual void OnBeforeModuleShutdown(IModule* module) = 0;
    virtual void OnAfterModuleShutdown(IModule* module) = 0;
    virtual void OnModuleNotFound(const ZString& string) = 0;
    virtual void SetOption(const ZString& string, const ZString& string2) = 0;
    virtual ZString GetOption(const ZString& string) = 0;
    virtual bool GetOptionBool(const ZString& string) = 0;
    virtual IIniFile* GetIniFile() = 0;
    virtual ERunMode GetRunMode() = 0;
    virtual void Exit() = 0;
    virtual void SetDebugContextInfo(const ZString& string, const ZString& string2) = 0;
    virtual ZString GetDebugContextInfo(const ZString& string) = 0;
    virtual bool IsNonInteractiveMode() = 0;
    virtual void SendErrorMail(const ZString& string, const ZString& string2) = 0;
    virtual void GetErrorMailSenderName(char* param1) = 0;
};

class IIniFile : public IComponentInterface
{
  public:
    virtual ~IIniFile() = 0;
    virtual bool Load(const ZFilePath& filePath) = 0;
    virtual bool LoadFromString(const ZString& string) = 0;
    virtual IIniFileSection* GetSection(const ZString& string) = 0;
    virtual TEnumerator<IIniFileSection*> GetSections() = 0;
    virtual ZString GetValue(const ZString& string, const ZString& string2) = 0;
    virtual void SetValue(const ZString& string, const ZString& string2, const ZString& string3) = 0;
    virtual TEnumerator<ZString> GetConsoleCmds() = 0;
};

class ZApplicationBase : public IApplication
{
  private:
    PAD(0x14);
};

class ZEngineAppCommon
{
  public:
    void CreateFreeCameraAndControl()
    {
        Functions::ZEngineAppCommon_CreateFreeCameraAndControl->Call(this);
    }

    void CopyMainCameraSettingsToFreeCamera()
    {
        const ZHM5MainCamera* mainCamera = Globals::LevelManager->m_rHitman.m_pInterfaceRef->m_rMainCamera.m_pInterfaceRef;
        ZCameraEntity* freeCamera = m_pFreeCamera.m_pInterfaceRef;

        freeCamera->SetObjectToWorldMatrix(mainCamera->GetObjectToWorldMatrix());
        freeCamera->SetClearColor(mainCamera->GetClearColor());
        freeCamera->m_fNearZ = mainCamera->GetNearZ();
        freeCamera->m_fFarZ = mainCamera->GetFarZ();
        freeCamera->SetFovYDeg(mainCamera->GetFovYDeg());
        freeCamera->SetPostfilter(mainCamera->GetPostfilter());
    }

    PAD(0x2C);
    TEntityRef<ZFreeCameraControlEntity> m_pFreeCameraControl; // 0x2C
    TEntityRef<ZCameraEntity> m_pFreeCamera;                   // 0x34
    TEntityRef<ZCameraEntity> m_pMainCamera;                   // 0x3C
    bool m_bIsGameInputActive;                                 // 0x44
    PAD(0x3B);
};

class ZWin32ApplicationStub : public ZApplicationBase
{
  public:
    virtual void Initialize(const ZString& sDefaultSettings) = 0;
    virtual void AddApplicationSpecificOptions(IIniFile* pIniFile) = 0;
    virtual bool Startup() = 0;
    virtual void Shutdown() = 0;

    void ApplyOptionOverrides(int32_t& p_Argc, char** p_Argv)
    {
        Functions::ZWin32ApplicationStub_ApplyOptionOverrides->Call(this, p_Argc, p_Argv);
    }

    PAD(0x4);                           // 0x18
    IIniFile* m_pIniFile;               // 0x1C
    TMap<ZString, ZString>* m_pOptions; // 0x20
    PAD(0x20);                          // 0x24
};

class ZWin32ApplicationBase : public ZWin32ApplicationStub
{
  public:
    virtual int Main(int param1, char** param2) = 0;

    PAD(0x1C);
};

class ZApplicationEngineWin32 : public ZWin32ApplicationBase
{
  public:
    static ZCameraEntity* GetActiveCamera()
    {
        ZFreeCameraControlEntity* freeCameraControlEntity = (*Globals::ApplicationEngineWin32)->m_common.m_pFreeCameraControl.m_pInterfaceRef;

        if (freeCameraControlEntity)
        {
            ZCameraEntity* freeCamera = (*Globals::ApplicationEngineWin32)->m_common.m_pFreeCamera.m_pInterfaceRef;

            if (freeCameraControlEntity->m_bActive)
            {
                return freeCamera;
            }

            TEntityRef<IRenderDestinationEntity> renderDestinationEntity = Globals::RenderManager->GetGameRenderDestinationEntity();
            TEntityRef<ZCameraEntity> currentCamera = renderDestinationEntity.m_pInterfaceRef->GetSource();

            if (currentCamera.m_pInterfaceRef == freeCamera)
            {
                return freeCamera;
            }
        }

        ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

        return hitman->m_rMainCamera.m_pInterfaceRef;
    }

    PAD(0x10);                    // 0x60
    ZEngineAppCommon m_common;    // 0x70
    PAD(0x64);                    // 0xF0
    HWND m_hWnd;                  // 0x154
    bool m_bTerminateApplication; // 0x158
    bool m_bShowingCursor;        // 0x159
    bool m_bTempWindowed;         // 0x15A
    bool m_bPauseGameLoop;        // 0x15B
    bool m_bGameLoopPaused;       // 0x15C
    int m_PreviousMasterVolume;   // 0x160
    bool m_bChangeMasterVolume;   // 0x164
    int m_TargetMasterVolume;     // 0x168
    bool m_bActive;               // 0x16C
    HICON m_hDefaultCursor;       // 0x170
    PAD(0x4);                     // 0x174
};

static_assert(sizeof(ZApplicationBase) == 0x18);
static_assert(alignof(ZApplicationBase) == 0x4);
static_assert(sizeof(ZWin32ApplicationStub) == 0x44);
static_assert(alignof(ZWin32ApplicationStub) == 0x4);
static_assert(sizeof(ZWin32ApplicationBase) == 0x60);
static_assert(alignof(ZWin32ApplicationBase) == 0x4);
static_assert(sizeof(ZApplicationEngineWin32) == 0x178);

class ZIniFile : public IIniFile
{
  public:
    virtual bool Load(const ZFilePath& path) = 0;
    virtual bool LoadFromString(const ZString& sSettings) = 0;
    virtual IIniFileSection* GetSection(const ZString& sName) = 0;
    virtual TEnumerator<IIniFileSection*> GetSections() = 0;
    virtual ZString GetValue(const ZString& sSectionName, const ZString& sOptionName) = 0;
    virtual void SetValue(const ZString& sSectionName, const ZString& sOptionName, const ZString& sValue) = 0;
    virtual TEnumerator<ZString> GetConsoleCmds() = 0;

    bool LoadFromStringInternal(const ZString& p_IniFileContent, const ZFilePath& p_Path)
    {
        return Functions::ZIniFile_LoadFromStringInternal->Call(this, p_IniFileContent, p_Path);
    }

    static bool LoadIniFileContent(const ZFilePath& p_PlainTextIniFilePath, TArray<unsigned char>& p_Buffer, bool p_PlainText)
    {
        std::string plainTextFilePath = p_PlainTextIniFilePath.ToString().ToCString();
        std::string filePathToRead;

        if (p_PlainText)
        {
            filePathToRead = plainTextFilePath;
        }
        else
        {
            filePathToRead = std::format("{}.scrambled", plainTextFilePath);
        }

        FILE* file = nullptr;

        fopen_s(&file, filePathToRead.c_str(), "rb");

        if (file)
        {
            fseek(file, 0, 2);

            unsigned int fileLength = ftell(file);

            fseek(file, 0, 0);

            if (fileLength <= p_Buffer.Size())
            {
                unsigned char* start = p_Buffer.GetStart();

                p_Buffer.m_pEnd = &start[fileLength];
            }
            else
            {
                if (fileLength > p_Buffer.Capacity())
                {
                    p_Buffer.Reserve(fileLength);
                }

                p_Buffer.m_pEnd = p_Buffer.GetEnd() + fileLength;
            }

            fread(p_Buffer.GetStart(), 1, fileLength, file);
            fclose(file);

            return true;
        }

        return false;
    }

    TMap<ZString, IIniFileSection*> m_sections;
    ZIniFileSection* m_pCurrentSection;
    TArray<ZString> m_ConsoleCmds;
    TArray<ZString> m_IniFilesLoaded;
};
