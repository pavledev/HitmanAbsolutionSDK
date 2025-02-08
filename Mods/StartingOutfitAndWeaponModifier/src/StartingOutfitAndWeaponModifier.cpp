#include <IconsMaterialDesign.h>

#include <Glacier/ContentKit/ZContentKitManager.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/Resource/ZResourceManager.h>
#include <Glacier/Resource/ZHeaderLibrary.h>

#include <StartingOutfitAndWeaponModifier.h>
#include "Hooks.h"
#include "Utility/ResourceUtility.h"
#include "Utility/MemoryUtility.h"

StartingOutfitAndWeaponModifier::OutfitKitEntity::~OutfitKitEntity()
{
    if (textureView)
    {
        textureView->Release();

        textureView = nullptr;
    }

    if (texture)
    {
        texture->Release();

        texture = nullptr;
    }
}

StartingOutfitAndWeaponModifier::FireArmKitEntity::~FireArmKitEntity()
{
    if (textureView)
    {
        textureView->Release();

        textureView = nullptr;
    }

    if (texture)
    {
        texture->Release();

        texture = nullptr;
    }
}

StartingOutfitAndWeaponModifier::StartingOutfitAndWeaponModifier()
{
    isOpen = false;
    selectedOutfitIndex = -1;
    selectedWeaponIndex = -1;
    selectedItemIndex = -1;
}

StartingOutfitAndWeaponModifier::~StartingOutfitAndWeaponModifier()
{
    Hooks::ZHitman5_ZHitman5.RemoveHook();
}

void StartingOutfitAndWeaponModifier::Initialize()
{
    ModInterface::Initialize();
}

void StartingOutfitAndWeaponModifier::OnEngineInitialized()
{
    Hooks::ZHitman5_ZHitman5.CreateHook("ZHitman5::ZHitman5", 0x21B130, ZHitman5_ZHitman5Hook);

    Hooks::ZHitman5_ZHitman5.EnableHook();
}

void StartingOutfitAndWeaponModifier::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_TUNE " Starting Outfit And Weapon Modifier"))
    {
        isOpen = !isOpen;
    }
}

void StartingOutfitAndWeaponModifier::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(1250, 850), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_TOKEN " Starting Outfit And Weapon Modifier", &isOpen, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        if (outfitKitEntities.size() == 0)
        {
            GetOutfitKitEntities();
        }

        if (fireArmKitEntities.size() == 0)
        {
            GetFireArmKitEntities();
        }

        if (ImGui::BeginTabBar("##TabBar"))
        {
            RenderOutfitsTabItem();
            RenderWeaponsTabItem();

            ImGui::EndTabBar();
        }

        ImGui::Separator();

        if (ImGui::Button("Ok"))
        {
            isOpen = false;
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

const STokenID& StartingOutfitAndWeaponModifier::GetSelectedOutfitTokenID() const
{
    if (selectedOutfitIndex != -1)
    {
        return outfitKitEntities[selectedOutfitIndex].tokenID;
    }

    return STokenID::InvalidToken;
}

const STokenID& StartingOutfitAndWeaponModifier::GetSelectedWeaponTokenID() const
{
    if (selectedWeaponIndex != -1)
    {
        return fireArmKitEntities[selectedWeaponIndex].tokenID;
    }

    return STokenID::InvalidToken;
}

void StartingOutfitAndWeaponModifier::RenderOutfitsTabItem()
{
    bool isTabVisible = ImGui::BeginTabItem("Outfits");

    if (isTabVisible)
    {
        ImGui::TextUnformatted("Select outfit:");
        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(600, 500), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Spacing();

        bool isOutfitSelected = false;
        const float checkboxHeight = ImGui::GetFrameHeight();

        for (size_t i = 0; i < outfitKitEntities.size(); ++i)
        {
            bool isItemSelected = selectedItemIndex == i;
            char itemLabel[32];

            snprintf(itemLabel, sizeof(itemLabel), "##Item %d", i);

            ImGui::SetNextItemAllowOverlap();

            if (ImGui::Selectable(itemLabel, &isItemSelected, 0, ImVec2(0, checkboxHeight)))
            {
                selectedItemIndex = i;
            }

            ImGui::SameLine();

            bool isCheckboxChecked = ImGui::Checkbox(outfitKitEntities[i].title, &outfitKitEntities[i].isSelected);

            if (isCheckboxChecked)
            {
                selectedOutfitIndex = i;
                isOutfitSelected = true;

                if (!isItemSelected)
                {
                    selectedItemIndex = i;
                }
            }
        }

        if (isOutfitSelected)
        {
            for (size_t i = 0; i < outfitKitEntities.size(); ++i)
            {
                if (i == selectedOutfitIndex)
                {
                    continue;
                }

                outfitKitEntities[i].isSelected = false;
            }
        }

        if (selectedItemIndex != -1)
        {
            OutfitKitEntity& outfitKitEntity = outfitKitEntities[selectedItemIndex];

            if (!outfitKitEntity.texture)
            {
                LoadDDSTexture(outfitKitEntity.hiResNotebookImage, &outfitKitEntity.texture, &outfitKitEntity.textureView, outfitKitEntity.width, outfitKitEntity.height);
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("ItemView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        if (selectedItemIndex != -1)
        {
            OutfitKitEntity& outfitKitEntity = outfitKitEntities[selectedItemIndex];

            ImGui::Image(outfitKitEntity.textureView, ImVec2(outfitKitEntity.width, outfitKitEntity.height));
            ImGui::SameLine();
            ImGui::TextWrapped(outfitKitEntity.description);
        }

        ImGui::EndChild();

        ImGui::EndTabItem();
    }
}

void StartingOutfitAndWeaponModifier::RenderWeaponsTabItem()
{
    bool isTabVisible = ImGui::BeginTabItem("Weapons");

    if (isTabVisible)
    {
        ImGui::TextUnformatted("Select weapon:");
        ImGui::Separator();

        ImGui::BeginChild("ScrollingRegion", ImVec2(600, 500), false, ImGuiWindowFlags_HorizontalScrollbar);
        ImGui::Spacing();

        bool isWeaponSelected = false;
        const float checkboxHeight = ImGui::GetFrameHeight();

        for (size_t i = 0; i < fireArmKitEntities.size(); ++i)
        {
            bool isItemSelected = selectedItemIndex == i;
            char itemLabel[32];

            snprintf(itemLabel, sizeof(itemLabel), "##Item %d", i);

            ImGui::SetNextItemAllowOverlap();

            if (ImGui::Selectable(itemLabel, &isItemSelected, 0, ImVec2(0, checkboxHeight)))
            {
                selectedItemIndex = i;
            }

            ImGui::SameLine();

            bool isCheckboxChecked = ImGui::Checkbox(fireArmKitEntities[i].title, &fireArmKitEntities[i].isSelected);

            if (isCheckboxChecked)
            {
                selectedWeaponIndex = i;
                isWeaponSelected = true;

                if (!isItemSelected)
                {
                    selectedItemIndex = i;
                }
            }
        }

        if (isWeaponSelected)
        {
            for (size_t i = 0; i < fireArmKitEntities.size(); ++i)
            {
                if (i == selectedWeaponIndex)
                {
                    continue;
                }

                fireArmKitEntities[i].isSelected = false;
            }
        }

        if (selectedItemIndex != -1)
        {
            FireArmKitEntity& fireArmKitEntity = fireArmKitEntities[selectedItemIndex];

            if (!fireArmKitEntity.texture)
            {
                LoadDDSTexture(fireArmKitEntity.hiResNotebookImage, &fireArmKitEntity.texture, &fireArmKitEntity.textureView, fireArmKitEntity.width, fireArmKitEntity.height);
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("ItemView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        if (selectedItemIndex != -1)
        {
            FireArmKitEntity& fireArmKitEntity = fireArmKitEntities[selectedItemIndex];

            ImGui::Image(fireArmKitEntity.textureView, ImVec2(fireArmKitEntity.width, fireArmKitEntity.height));
            ImGui::SameLine();
            ImGui::TextWrapped(fireArmKitEntity.description);
        }

        ImGui::EndChild();

        ImGui::EndTabItem();
    }
}

void StartingOutfitAndWeaponModifier::GetOutfitKitEntities()
{
    const ZContentKitList<ZOutfitKitEntity, ZOutfitKitMenuSetup, SOutfitKitSaveData, SOutfitKitState>& outfits = ContentKitManager->GetOutfits();
    const ZShadowContentKitList& shadowContentKitList = outfits.GetShadowContentKitList();
    const TArray<ZShadowContentKitList::SEntry>& entries = shadowContentKitList.GetEntries();

    outfitKitEntities.resize(entries.Size());

    for (size_t i = 0; i < entries.Size(); ++i)
    {
        const ZOutfitKitEntity* outfitKitEntity = static_cast<ZOutfitKitEntity*>(entries[i].GetFirstKit().GetRawPointer());
        OutfitKitEntity& outfitKitEntity2 = outfitKitEntities[i];

        outfitKitEntity2.title = outfitKitEntity->GetLocalizedTitle().ToCString();
        outfitKitEntity2.description = outfitKitEntity->GetLocalizedDescription().ToCString();
        outfitKitEntity2.isSelected = false;
        outfitKitEntity2.tokenID = outfitKitEntity->GetTokenID();
        outfitKitEntity2.hiResNotebookImage = outfitKitEntity->GetLargeIconRID();
    }
}

void StartingOutfitAndWeaponModifier::GetFireArmKitEntities()
{
    const ZContentKitList<ZFireArmKitEntity, ZFireArmKitMenuSetup, SFireArmKitSaveData, SFireArmKitState>& fireArms = ContentKitManager->GetFireArms();
    const ZShadowContentKitList& shadowContentKitList = fireArms.GetShadowContentKitList();
    const TArray<ZShadowContentKitList::SEntry>& entries = shadowContentKitList.GetEntries();

    fireArmKitEntities.resize(entries.Size());

    for (size_t i = 0; i < entries.Size(); ++i)
    {
        const ZFireArmKitEntity* fireArmKitEntity = static_cast<ZFireArmKitEntity*>(entries[i].GetFirstKit().GetRawPointer());
        FireArmKitEntity& fireArmKitEntity2 = fireArmKitEntities[i];

        fireArmKitEntity2.title = fireArmKitEntity->GetLocalizedTitle().ToCString();
        fireArmKitEntity2.description = fireArmKitEntity->GetLocalizedDescription().ToCString();
        fireArmKitEntity2.isSelected = false;
        fireArmKitEntity2.tokenID = fireArmKitEntity->GetTokenID();
        fireArmKitEntity2.hiResNotebookImage = fireArmKitEntity->GetLargeIconRID();
    }
}

void StartingOutfitAndWeaponModifier::LoadDDSTexture(const ZRuntimeResourceID& hiResNotebookImage, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, float& width, float& height)
{
    TResourcePtr<ZHeaderLibrary> headerLibraryResourcePtr = ResourceManager->LoadResource(hiResNotebookImage);
    const ZResourcePtr tempResourcePtr = headerLibraryResourcePtr->GetSourceResource();
    const ZRuntimeResourceID headerLibraryRuntimeResourceID = headerLibraryResourcePtr.GetResourceStub()->GetRuntimeResourceID();
    const ZRuntimeResourceID libraryRuntimeResourceID = tempResourcePtr.GetResourceStub()->GetRuntimeResourceID().GetLibraryRuntimeResourceID();
    const ZRuntimeResourceID swffRuntimeResourceID = ZRuntimeResourceID::CreateLibraryResourceID(libraryRuntimeResourceID, 2);
    void* resourceData = nullptr;
    unsigned int resourceDataSize = 0;

    ResourceUtility::LoadResource(swffRuntimeResourceID, headerLibraryRuntimeResourceID, resourceData, resourceDataSize);

    BinaryReader binaryReader = BinaryReader(resourceData, resourceDataSize);

    binaryReader.Skip(1);

    unsigned int fileOffset = binaryReader.Read<unsigned int>();

    binaryReader.Seek(fileOffset - 4, SeekOrigin::Current);

    void* ddsTextureData = binaryReader.GetBuffer(true);
    unsigned int ddsTextureDataSize = resourceDataSize - binaryReader.GetPosition();

    DirectXRenderer::CreateDDSTextureFromMemory(ddsTextureData, ddsTextureDataSize, texture, textureView, width, height);

    FreeMemory(resourceData);
}

void __fastcall ZHitman5_ZHitman5Hook(ZHitman5* pThis, int edx, ZComponentCreateInfo& pInfo)
{
    SSceneParameters& sceneParameters = LevelManager->GetSceneParameters();
    const STokenID& selectedOutfitTokenID = GetModInstance()->GetSelectedOutfitTokenID();
    const STokenID& selectedWeaponTokenID = GetModInstance()->GetSelectedWeaponTokenID();

    if (selectedOutfitTokenID != STokenID::InvalidToken)
    {
        sceneParameters.BonusOutfit = selectedOutfitTokenID;
    }

    if (selectedWeaponTokenID != STokenID::InvalidToken)
    {
        sceneParameters.BonusWeapon = selectedWeaponTokenID;
    }

    Hooks::ZHitman5_ZHitman5.CallOriginalFunction(pThis, pInfo);
}

DEFINE_MOD(StartingOutfitAndWeaponModifier);
