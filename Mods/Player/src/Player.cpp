#include <fstream>

#include <imgui_internal.h>

#include <IconsMaterialDesign.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>

#include <Glacier/ZLevelManager.h>
#include <Glacier/ContentKit/ZContentKitManager.h>
#include <Glacier/Resource/ZResourceManager.h>
#include <Glacier/Resource/ZHeaderLibrary.h>
#include <Glacier/Actor/ZActorManager.h>
#include <Glacier/Input/ZInputActionManager.h>
#include <Glacier/Resource/ZDynamicResourceLibrary.h>
#include <Glacier/Item/ZHM5Item.h>
#include <Glacier/CheckPoint/ZCheckPointManager.h>
#include <Glacier/Entity/ZTemplateEntityFactory.h>
#include <Glacier/Entity/ZAspectEntityFactory.h>

#include <Player.h>
#include <Global.h>
#include <Utility/ResourceUtility.h>
#include <Utility/MemoryUtility.h>
#include "Resource/DynamicHeaderLibraryGenerator.h"
#include <Hooks.h>

Player::OutfitKitEntity::~OutfitKitEntity()
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

Player::FireArmKitEntity::~FireArmKitEntity()
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

Player::PropKitEntity::~PropKitEntity()
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

Player::Player()
{
    isOpen = false;

    isGodModeEnabled = false;
    isInvisibilityEnabled = false;
    isInfAmmoEnabled = false;

    selectedOutfitIndex = -1;
    selectedWeaponIndex = -1;
    selectedItemIndex = -1;

    getOutfitAction = ZInputAction("GetOufit");
    getModelAction = ZInputAction("GetModel");
    changeOutfitAction = ZInputAction("ChangeOutfit");
    teleportAction = ZInputAction("TeleportHitman");

    spawnWeapon = false;
    spawnItem = false;
    spawnProp = false;
    spawnActor = false;
    weaponCount = 0;
    itemCount = 0;
    propCount = 0;
    actorCount = 0;
    addWeaponToWorld = false;
    addItemToWorld = false;
    spawnCivilianActor = false;
    selectedActorWeaponIndex = -1;
}

Player::~Player()
{
    const ZMemberDelegate<Player, void(const SGameUpdateEvent&)> delegate(this, &Player::OnFrameUpdate);

    GameLoopManager->UnregisterForFrameUpdate(delegate);

    Hooks::ZEngineAppCommon_DefaultMainLoopSequence.DisableHook();
    Hooks::ZEngineAppCommon_DefaultMainLoopSequence.RemoveHook();

    for (size_t i = 0; i < dynamicResourceLibraries.size(); ++i)
    {
        FreeObject(dynamicResourceLibraries[i]);
    }
}

void Player::Initialize()
{
    ModInterface::Initialize();

    Hooks::ZEngineAppCommon_DefaultMainLoopSequence.CreateHook("ZEngineAppCommon::DefaultMainLoopSequence", 0x4C7580, ZEngineAppCommon_DefaultMainLoopSequenceHook);
    Hooks::ZEntityManager_ConstructUninitializedEntity.CreateHook("ZEntityManager::ConstructUninitializedEntity", 0x565200, ZEntityManager_ConstructUninitializedEntityHook);

    Hooks::ZEngineAppCommon_DefaultMainLoopSequence.EnableHook();
    Hooks::ZEntityManager_ConstructUninitializedEntity.EnableHook();

    godMode = reinterpret_cast<int*>(BaseAddress + 0xD4F5E0);
    invisible = reinterpret_cast<int*>(BaseAddress + 0xD54328);
    infAmmo = reinterpret_cast<int*>(BaseAddress + 0xD4DBB4);
}

void Player::OnEngineInitialized()
{
    const ZMemberDelegate<Player, void(const SGameUpdateEvent&)> delegate(this, &Player::OnFrameUpdate);

    GameLoopManager->RegisterForFrameUpdate(delegate, 1);

    const char* bindings = "PlayerModInput={"
        "GetOufit=tap(kb,i);"
        "GetModel=tap(kb,o);"
        "ChangeOutfit=tap(kb,p);"
        "TeleportHitman=tap(kb,j);};";

    InputActionManager->AddBindings(bindings);
}

void Player::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_MAN " Player"))
    {
        isOpen = !isOpen;
    }
}

void Player::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_TOKEN " Player", &isOpen, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        if (outfitKitEntities.size() == 0)
        {
            GetOufits();
        }

        if (fireArmKitEntities.size() == 0)
        {
            GetWeapons();
        }

        if (propKitEntities.size() == 0)
        {
            GetItems();
        }

        if (ImGui::BeginTabBar("##TabBar"))
        {
            RenderCheatsTabItem();
            RenderOutfitsTabItem();
            RenderWeaponsTabItem();
            RenderItemsTabItem();
            RenderPropsTabItem();
            RenderActorsTabItem();

            ImGui::EndTabBar();
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Player::OnDefaultMainLoopSequence()
{
    if (spawnWeapon)
    {
        SpawnWeapon(fireArmKitEntities[selectedWeaponIndex].runtimeResourceID, weaponCount, addWeaponToWorld);

        if (!addWeaponToWorld)
        {
            spawnWeapon = false;
        }
    }
    else if (spawnItem)
    {
        SpawnItem(propKitEntities[selectedItemIndex].runtimeResourceID, itemCount, addItemToWorld);

        if (!addItemToWorld)
        {
            spawnItem = false;
        }
    }
    else if (spawnProp)
    {
        SpawnProp();
    }
    else if (spawnActor)
    {
        SpawnActor();
    }
}

void Player::OnConstructUninitializedEntity(IEntityFactory* pEntityFactory, ZEntityType** entityType)
{
    if ((addWeaponToWorld || addItemToWorld || spawnProp) && ZTemplateEntityFactoryVFTbl == *reinterpret_cast<void**>(pEntityFactory))
    {
        ZTemplateEntityFactory* templateEntityFactory = static_cast<ZTemplateEntityFactory*>(pEntityFactory);
        const ZRuntimeResourceID templateRuntimeResourceID = templateEntityFactory->GetTemplateRuntimeResourceID().GetID();

        if (templateRuntimeResourceID == sourceResourceRuntimeResourceID)
        {
            ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

            if (hitman)
            {
                ZEntityRef entityRef = ZEntityRef(entityType);
                ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().GetRawPointer();
                SMatrix43 transform;

                transform.Trans = hitmanSpatialEntity->GetWorldPosition() - float4(0.f, 0.5f, 0.f, 0.f);

                entityRef.SetProperty("m_mTransform", transform);
            }
        }

        static unsigned int spawnedWeaponCount = 1;
        static unsigned int spawnedItemCount = 1;
        static unsigned int spawnedPropCount = 1;

        if (spawnWeapon)
        {
            if (spawnedWeaponCount == weaponCount)
            {
                spawnedWeaponCount = 1;

                if (spawnWeapon)
                {
                    spawnWeapon = false;
                }
            }
            else
            {
                ++spawnedWeaponCount;
            }
        }
        else if (spawnItem)
        {
            if (spawnedItemCount == itemCount)
            {
                spawnedItemCount = 1;

                if (spawnItem)
                {
                    spawnItem = false;
                }
            }
            else
            {
                ++spawnedItemCount;
            }
        }
        else if (spawnProp)
        {
            if (spawnedPropCount == propCount)
            {
                spawnedPropCount = 1;

                if (spawnProp)
                {
                    spawnProp = false;
                }
            }
            else
            {
                ++spawnedPropCount;
            }
        }

    }
    else if (spawnActor && ZAspectEntityFactoryVFTbl == *reinterpret_cast<void**>(pEntityFactory))
    {
        ZAspectEntityFactory* aspectEntityFactory = static_cast<ZAspectEntityFactory*>(pEntityFactory);
        const ZRuntimeResourceID asetRuntimeResourceID = aspectEntityFactory->GetAspectEntityRuntimeResourceID();

        if (asetRuntimeResourceID == sourceResourceRuntimeResourceID)
        {
            ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

            if (hitman)
            {
                ZEntityRef entityRef = ZEntityRef(entityType);
                ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().GetRawPointer();
                SMatrix43 transform;

                transform.Trans = hitmanSpatialEntity->GetWorldPosition() - float4(0.f, 0.5f, 0.f, 0.f);

                entityRef.SetProperty("m_bStartEnabled", true);
                entityRef.SetProperty("m_mTransform", transform);
            }
        }

        static unsigned int spawnedActorCount = 1;

        if (spawnActor)
        {
            if (spawnedActorCount == actorCount)
            {
                spawnedActorCount = 1;

                if (spawnActor)
                {
                    spawnActor = false;
                }
            }
            else
            {
                ++spawnedActorCount;
            }
        }
    }
}

void Player::OnFrameUpdate(const SGameUpdateEvent& updateEvent)
{
    if (getOutfitAction.Digital())
    {
        ZActor* actor = FindNearestActor();

        if (actor)
        {
            EquipOutfit(actor);
        }
    }

    if (getModelAction.Digital())
    {
        ZActor* actor = FindNearestActor();

        if (actor)
        {
            EquipModel(actor);
        }
    }

    if (changeOutfitAction.Digital())
    {
        if (outfitKitEntities.size() == 0)
        {
            GetOufits();
        }

        static unsigned int outfitIndex = 0;
        static unsigned int outfitVariationIndex = 0;

        if (outfitVariationIndex == outfitKitEntities[outfitIndex].outfitVariations.size())
        {
            ++outfitIndex;
            outfitVariationIndex = 0;
        }

        if (outfitIndex == outfitKitEntities.size())
        {
            outfitIndex = 0;
        }

        EquipOutfit(outfitIndex, outfitVariationIndex);

        ++outfitVariationIndex;
    }

    if (teleportAction.Digital())
    {
        ZCheckPointManagerEntity* checkPointManagerEntity = CheckPointManager->GetCheckPointManagerEntity().GetRawPointer();

        checkPointManagerEntity->ActivateJumpPoint(checkPointManagerEntity->GetCurrentJumpPoint() + 1, true);
    }
}

void Player::RenderCheatsTabItem()
{
    bool isTabVisible = ImGui::BeginTabItem("Cheats");

    if (isTabVisible)
    {
        if (ImGui::Checkbox("God Mode", &isGodModeEnabled))
        {
            *godMode = static_cast<int>(isGodModeEnabled);
        }

        if (ImGui::Checkbox("Invisible", &isInvisibilityEnabled))
        {
            *invisible = static_cast<int>(isInvisibilityEnabled);
        }

        if (ImGui::Checkbox("Infinite Ammo", &isInfAmmoEnabled))
        {
            *infAmmo = static_cast<int>(isInfAmmoEnabled);
        }

        if (ImGui::Button("Refill Focus"))
        {
            if (LevelManager)
            {
                ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

                if (hitman)
                {
                    hitman->GetFocusController()->SetFocus(1.0f);
                }
            }
        }

        ImGui::EndTabItem();
    }
}

void Player::RenderOutfitsTabItem()
{
    bool isTabVisible = ImGui::BeginTabItem("Outfits");

    if (!isTabVisible)
    {
        return;
    }

    ImGui::BeginChild("OutfitList", ImVec2(600, 500), false, ImGuiWindowFlags_HorizontalScrollbar);

    static char outfitName[256]{ "" };
    static bool showOnlyOutfitsOfCurrentScene = false;

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Outfit Name");
    ImGui::SameLine();
    ImGui::InputText("##OutfitName", outfitName, sizeof(outfitName));

    const float checkboxHeight = ImGui::GetFrameHeight();
    static char currentOutfitVariationIndex[2]{ "0" };

    for (size_t i = 0; i < outfitKitEntities.size(); ++i)
    {
        if (!strstr(outfitKitEntities[i].title.c_str(), outfitName))
        {
            continue;
        }

        if (showOnlyOutfitsOfCurrentScene && !outfitKitEntities[i].isOutfitForCurrentScene)
        {
            continue;
        }

        bool isOufitSelected = selectedOutfitIndex == i;

        if (ImGui::Selectable(outfitKitEntities[i].title.c_str(), &isOufitSelected))
        {
            selectedOutfitIndex = i;

            strcpy_s(currentOutfitVariationIndex, "0");

            OutfitKitEntity& outfitKitEntity = outfitKitEntities[selectedOutfitIndex];

            if (!outfitKitEntity.texture)
            {
                LoadDDSTexture(outfitKitEntity.hiResNotebookImage, &outfitKitEntity.texture, &outfitKitEntity.textureView, outfitKitEntity.width, outfitKitEntity.height);
            }
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("ItemView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

    if (selectedOutfitIndex != -1)
    {
        const OutfitKitEntity& outfitKitEntity = outfitKitEntities[selectedOutfitIndex];

        ImGui::Image(outfitKitEntity.textureView, ImVec2(outfitKitEntity.width, outfitKitEntity.height));
        ImGui::SameLine();

        ImGui::BeginChild("ItemView2", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        ImGui::TextWrapped(outfitKitEntity.description.c_str());

        ImGui::Spacing();
        ImGui::AlignTextToFramePadding();
        ImGui::Text("Outfit Variation Index");
        ImGui::SameLine();

        if (ImGui::BeginCombo("##OutfitVariationIndex", currentOutfitVariationIndex))
        {
            for (size_t i = 0; i < outfitKitEntity.outfitVariations.size(); ++i)
            {
                const std::string outfitVariationIndex = std::to_string(i);
                const bool isOutfitVarationSelected = currentOutfitVariationIndex == outfitVariationIndex.c_str();

                if (ImGui::Selectable(outfitVariationIndex.c_str(), isOutfitVarationSelected))
                {
                    strcpy_s(currentOutfitVariationIndex, outfitVariationIndex.c_str());
                }
            }

            ImGui::EndCombo();
        }

        ImGui::EndChild();
    }

    ImGui::EndChild();

    ImGui::Checkbox("Show only outfits of current scene", &showOnlyOutfitsOfCurrentScene);
    ImGui::Spacing();

    if (ImGui::Button("Equip Outfit"))
    {
        if (selectedOutfitIndex != -1)
        {
            EquipOutfit(selectedOutfitIndex, std::stoi(currentOutfitVariationIndex));
        }
    }

    ImGui::Separator();

    static int button = 1;

    ImGui::Text("Get Outfit/Model From Actor");
    ImGui::Spacing();

    if (ImGui::RadioButton("Oufit", button == 1))
    {
        button = 1;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Model", button == 2))
    {
        button = 2;
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Name");
    ImGui::SameLine();

    static char actorName[100]{ "" };
    const bool isInputTextEnterPressed = ImGui::InputText("##ActorName", actorName, sizeof(actorName), ImGuiInputTextFlags_EnterReturnsTrue);
    const bool isInputTextActive = ImGui::IsItemActive();

    if (ImGui::IsItemActivated())
    {
        ImGui::OpenPopup("##Popup");
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));
    }

    if (ImGui::BeginPopup("##Popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
    {
        TArrayRef<TEntityRef<ZActor>> actors = ActorManager->GetAliveActors();

        for (size_t i = 0; i < actors.Size(); ++i)
        {
            ZActor* actor = actors[i].GetRawPointer();
            const ZString& actorName2 = actor->GetActorName();

            if (!strstr(actorName2.ToCString(), actorName))
            {
                continue;
            }

            if (ImGui::Selectable(actorName2.ToCString()))
            {
                ImGui::ClearActiveID();
                strcpy_s(actorName, actorName2.ToCString());

                ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

                if (button == 1)
                {
                    EquipOutfit(actor);
                }
                else
                {
                    EquipModel(actor);
                }
            }
        }

        if (isInputTextEnterPressed || (!isInputTextActive && !ImGui::IsWindowFocused()))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::Separator();

    static char resourceID[512]{ "" };

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Resource ID");
    ImGui::SameLine();
    ImGui::InputText("##ResourceID", resourceID, sizeof(resourceID), ImGuiInputTextFlags_EnterReturnsTrue);

    if (ImGui::Button("Equip Model"))
    {
        EquipModel(resourceID);
    }

    ImGui::Separator();

    if (ImGui::Button("Get Nearest Actor's Outfit"))
    {
        ZActor* actor = FindNearestActor();

        if (actor)
        {
            EquipOutfit(actor);
        }
    }

    if (ImGui::Button("Get Nearest Actor's Model"))
    {
        ZActor* actor = FindNearestActor();

        if (actor)
        {
            EquipModel(actor);
        }
    }

    ImGui::EndTabItem();
}

void Player::RenderWeaponsTabItem()
{
    bool isTabVisible = ImGui::BeginTabItem("Weapons");

    if (!isTabVisible)
    {
        return;
    }

    ImGui::BeginChild("WeaponList", ImVec2(600, 500), false, ImGuiWindowFlags_HorizontalScrollbar);

    static char weaponName[256]{ "" };

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Weapon Name");
    ImGui::SameLine();
    ImGui::InputText("##WeaponName", weaponName, sizeof(weaponName));

    const float checkboxHeight = ImGui::GetFrameHeight();

    for (size_t i = 0; i < fireArmKitEntities.size(); ++i)
    {
        if (!strstr(fireArmKitEntities[i].title.c_str(), weaponName))
        {
            continue;
        }

        bool isWeaponSelected = selectedWeaponIndex == i;

        if (ImGui::Selectable(fireArmKitEntities[i].title.c_str(), &isWeaponSelected))
        {
            selectedWeaponIndex = i;

            FireArmKitEntity& fireArmKitEntity = fireArmKitEntities[selectedWeaponIndex];

            if (!fireArmKitEntity.texture)
            {
                LoadDDSTexture(fireArmKitEntity.hiResNotebookImage, &fireArmKitEntity.texture, &fireArmKitEntity.textureView, fireArmKitEntity.width, fireArmKitEntity.height);
            }
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("ItemView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

    if (selectedWeaponIndex != -1)
    {
        const FireArmKitEntity& fireArmKitEntity = fireArmKitEntities[selectedWeaponIndex];

        ImGui::Image(fireArmKitEntity.textureView, ImVec2(fireArmKitEntity.width, fireArmKitEntity.height));
        ImGui::SameLine();
        ImGui::TextWrapped(fireArmKitEntity.description.c_str());
    }

    ImGui::EndChild();

    ImGui::Spacing();

    static int button = 1;

    if (ImGui::RadioButton("Add To World", button == 1))
    {
        button = 1;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Add To Inventory", button == 2))
    {
        button = 2;
    }

    static char numberOfWeaponsToSpawn[4]{ "1" };

    ImGui::Spacing();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Number of weapons to spawn");
    ImGui::SameLine();
    ImGui::InputText("##NumberOfWeaponsToSpawn", numberOfWeaponsToSpawn, sizeof(numberOfWeaponsToSpawn));
    ImGui::Spacing();

    if (ImGui::Button("Spawn Weapon"))
    {
        if (selectedWeaponIndex != -1)
        {
            weaponCount = std::atoi(numberOfWeaponsToSpawn);
            addWeaponToWorld = button == 1;
            spawnWeapon = true;
        }
    }

    ImGui::EndTabItem();
}

void Player::RenderItemsTabItem()
{
    bool isTabVisible = ImGui::BeginTabItem("Items");

    if (!isTabVisible)
    {
        return;
    }

    ImGui::BeginChild("ItemList", ImVec2(600, 500), false, ImGuiWindowFlags_HorizontalScrollbar);

    static char itemName[256]{ "" };

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Item Name");
    ImGui::SameLine();
    ImGui::InputText("##ItemName", itemName, sizeof(itemName));

    const float checkboxHeight = ImGui::GetFrameHeight();

    for (size_t i = 0; i < propKitEntities.size(); ++i)
    {
        if (!strstr(propKitEntities[i].title.c_str(), itemName))
        {
            continue;
        }

        bool isItemSelected = selectedItemIndex == i;

        if (ImGui::Selectable(propKitEntities[i].title.c_str(), &isItemSelected))
        {
            selectedItemIndex = i;

            PropKitEntity& propKitEntity = propKitEntities[selectedItemIndex];

            if (!propKitEntity.texture)
            {
                LoadDDSTexture(propKitEntity.hiResNotebookImage, &propKitEntity.texture, &propKitEntity.textureView, propKitEntity.width, propKitEntity.height);
            }
        }
    }

    ImGui::EndChild();

    ImGui::SameLine();
    ImGui::BeginChild("ItemView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

    if (selectedItemIndex != -1)
    {
        const PropKitEntity& propKitEntity = propKitEntities[selectedItemIndex];

        ImGui::Image(propKitEntity.textureView, ImVec2(propKitEntity.width, propKitEntity.height));
        ImGui::SameLine();
        ImGui::TextWrapped(propKitEntity.description.c_str());
    }

    ImGui::EndChild();

    ImGui::Spacing();

    static int button = 1;

    if (ImGui::RadioButton("Add To World", button == 1))
    {
        button = 1;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Add To Inventory", button == 2))
    {
        button = 2;
    }

    static char numberOfItemsToSpawn[4]{ "1" };

    ImGui::Spacing();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Number of items to spawn");
    ImGui::SameLine();
    ImGui::InputText("##NumberOfItemsToSpawn", numberOfItemsToSpawn, sizeof(numberOfItemsToSpawn));
    ImGui::Spacing();

    if (ImGui::Button("Spawn Item"))
    {
        if (selectedItemIndex != -1)
        {
            itemCount = std::atoi(numberOfItemsToSpawn);
            addItemToWorld = button == 1;
            spawnItem = true;
        }
    }

    ImGui::EndTabItem();
}

void Player::RenderPropsTabItem()
{
    bool isTabVisible = ImGui::BeginTabItem("Props");

    if (!isTabVisible)
    {
        return;
    }

    static char numberOfPropsToSpawn[4]{ "1" };

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Prop Resource ID");
    ImGui::SameLine();
    ImGui::InputText("##PropResourceID", propResourceID, sizeof(propResourceID));

    ImGui::Spacing();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Number of props to spawn");
    ImGui::SameLine();
    ImGui::InputText("##NumberOfPropsToSpawn", numberOfPropsToSpawn, sizeof(numberOfPropsToSpawn));
    ImGui::Spacing();

    if (ImGui::Button("Spawn Prop"))
    {
        propCount = std::atoi(numberOfPropsToSpawn);
        spawnProp = true;
    }

    ImGui::EndTabItem();
}

void Player::RenderActorsTabItem()
{
    bool isTabVisible = ImGui::BeginTabItem("Actors");

    if (!isTabVisible)
    {
        return;
    }

    static char numberOfActorsToSpawn[4]{ "1" };

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Actor Resource ID");
    ImGui::SameLine();
    ImGui::InputText("##ActorsResourceID", actorResourceID, sizeof(actorResourceID));

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Actor Name");
    ImGui::SameLine();
    ImGui::InputText("##ActorName", actorName, sizeof(actorName));

    static int button = 1;

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Actor Type");
    ImGui::Spacing();

    if (ImGui::RadioButton("Civilian", button == 1))
    {
        button = 1;
    }

    ImGui::SameLine();

    if (ImGui::RadioButton("Guard", button == 2))
    {
        button = 2;
    }

    ImGui::AlignTextToFramePadding();
    ImGui::Text("Weapon Name");
    ImGui::SameLine();

    static char actorWeaponName[50]{ "" };
    const bool isInputTextEnterPressed = ImGui::InputText("##WeaponName", actorWeaponName, sizeof(actorWeaponName), ImGuiInputTextFlags_EnterReturnsTrue);
    const bool isInputTextActive = ImGui::IsItemActive();

    if (ImGui::IsItemActivated())
    {
        ImGui::OpenPopup("##Popup");
        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));
    }

    if (ImGui::BeginPopup("##Popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow))
    {
        for (size_t i = 0; i < fireArmKitEntities.size(); ++i)
        {
            if (!strstr(fireArmKitEntities[i].title.c_str(), actorWeaponName))
            {
                continue;
            }

            if (ImGui::Selectable(fireArmKitEntities[i].title.c_str()))
            {
                ImGui::ClearActiveID();
                strcpy_s(actorWeaponName, fireArmKitEntities[i].title.c_str());

                selectedActorWeaponIndex = i;
            }
        }

        if (isInputTextEnterPressed || (!isInputTextActive && !ImGui::IsWindowFocused()))
        {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }

    ImGui::Spacing();
    ImGui::AlignTextToFramePadding();
    ImGui::Text("Number of actors to spawn");
    ImGui::SameLine();
    ImGui::InputText("##NumberOfActorsToSpawn", numberOfActorsToSpawn, sizeof(numberOfActorsToSpawn));
    ImGui::Spacing();

    if (ImGui::Button("Spawn Actor"))
    {
        actorCount = std::atoi(numberOfActorsToSpawn);
        spawnActor = true;
        spawnCivilianActor = button == 1;
    }

    ImGui::EndTabItem();
}

void Player::GetOufits()
{
    std::ifstream inputFile = std::ifstream("assets/Outfits.json");
    rapidjson::IStreamWrapper streamWrapper(inputFile);
    rapidjson::Document document;

    document.ParseStream(streamWrapper);

    const rapidjson::Value& outfitsArray = document["outfits"].GetArray();

    outfitKitEntities.resize(outfitsArray.Size());

    unsigned int outfitKitEntityIndex = 0;

    for (rapidjson::Value::ConstValueIterator it = outfitsArray.Begin(); it != outfitsArray.End(); ++it)
    {
        const rapidjson::Value& object = it->GetObj();
        OutfitKitEntity& outfitKitEntity = outfitKitEntities[outfitKitEntityIndex++];

        outfitKitEntity.title = object["title"].GetString();
        outfitKitEntity.description = object["description"].GetString();
        outfitKitEntity.tokenID = STokenID(object["hash"].GetUint());
        outfitKitEntity.hiResNotebookImage = object["hiResNotebookImage"].GetUint64();
    }

    const ZContentKitList<ZOutfitKitEntity, ZOutfitKitMenuSetup, SOutfitKitSaveData, SOutfitKitState>& outfits = ContentKitManager->GetOutfits();
    const ZShadowContentKitList& shadowContentKitList = outfits.GetShadowContentKitList();
    const TArray<ZShadowContentKitList::SEntry>& entries = shadowContentKitList.GetEntries();

    for (size_t i = 0; i < outfitKitEntities.size(); ++i)
    {
        OutfitKitEntity& outfitKitEntity = outfitKitEntities[i];

        for (size_t j = 0; j < entries.Size(); ++j)
        {
            const ZOutfitKitEntity* outfitKitEntity2 = static_cast<ZOutfitKitEntity*>(entries[j].GetFirstKit().GetRawPointer());

            if (outfitKitEntity.tokenID == outfitKitEntity2->GetTokenID())
            {
                outfitKitEntity.isOutfitForCurrentScene = true;

                break;
            }
        }
    }

    outfitKitEntityIndex = 0;

    const TMap<STokenID const, TEntityRef<ZGlobalOutfitKit>>& globalOutfitKits = ContentKitManager->GetGlobalOutfitKits();

    for (auto it = globalOutfitKits.Begin(); it != globalOutfitKits.End(); ++it)
    {
        const STokenID key = it.Node().Data().Key();
        ZGlobalOutfitKit* globalOutfitKit = it.Node().Data().Value().GetRawPointer();

        for (size_t i = 0; i < outfitKitEntities.size(); ++i)
        {
            OutfitKitEntity& outfitKitEntity = outfitKitEntities[i];

            if (outfitKitEntity.tokenID == key)
            {
                for (unsigned int j = 0; j < 12; ++j)
                {
                    const ZRuntimeResourceID variationResource = globalOutfitKit->GetVariationResource(j + 1);

                    if (variationResource.GetID() != -1)
                    {
                        outfitKitEntity.outfitVariations.push_back(variationResource);
                    }
                }

                break;
            }
        }
    }
}

void Player::GetWeapons()
{
    std::ifstream inputFile = std::ifstream("assets/Weapons.json");
    rapidjson::IStreamWrapper streamWrapper(inputFile);
    rapidjson::Document document;

    document.ParseStream(streamWrapper);

    const rapidjson::Value& weaponsArray = document["weapons"].GetArray();

    fireArmKitEntities.resize(weaponsArray.Size());

    unsigned int fireArmKitEntityIndex = 0;

    for (rapidjson::Value::ConstValueIterator it = weaponsArray.Begin(); it != weaponsArray.End(); ++it)
    {
        const rapidjson::Value& object = it->GetObj();
        FireArmKitEntity& fireArmKitEntity = fireArmKitEntities[fireArmKitEntityIndex++];

        fireArmKitEntity.title = object["title"].GetString();
        fireArmKitEntity.description = object["description"].GetString();
        fireArmKitEntity.hiResNotebookImage = object["hiResNotebookImage"].GetUint64();
        fireArmKitEntity.runtimeResourceID = object["runtimeResourceID"].GetUint64();
    }
}

void Player::GetItems()
{
    std::ifstream inputFile = std::ifstream("assets/Items.json");
    rapidjson::IStreamWrapper streamWrapper(inputFile);
    rapidjson::Document document;

    document.ParseStream(streamWrapper);

    const rapidjson::Value& itemsArray = document["items"].GetArray();

    propKitEntities.resize(itemsArray.Size());

    unsigned int propKitEntityIndex = 0;

    for (rapidjson::Value::ConstValueIterator it = itemsArray.Begin(); it != itemsArray.End(); ++it)
    {
        const rapidjson::Value& object = it->GetObj();
        PropKitEntity& propKitEntity = propKitEntities[propKitEntityIndex++];

        propKitEntity.title = object["title"].GetString();
        propKitEntity.description = object["description"].GetString();
        propKitEntity.hiResNotebookImage = object["hiResNotebookImage"].GetUint64();
        propKitEntity.runtimeResourceID = object["runtimeResourceID"].GetUint64();
    }
}

void Player::EquipOutfit(ZActor* actor)
{
    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    if (actor->IsWearingOutfit())
    {
        hitman->EquipOutfitResource(actor->GetHMAsResourceID(), actor->GetOutfit(), false, false);
    }
}

void Player::EquipOutfit(const unsigned int outfitIndex, const unsigned int outfitVariationIndex)
{
    const OutfitKitEntity& outfitKitEntity = outfitKitEntities[outfitIndex];
    const ZRuntimeResourceID outfitVariation = outfitKitEntity.outfitVariations[outfitVariationIndex];

    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();
    TEntityRef<ZHM5Outfit> outfit = LevelManager->GetOutfitManager().GetRawPointer()->GetOutfit(outfitKitEntity.tokenID);
    TEntityRef<IHM5Outfit> outfit2;

    if (outfit.GetRawPointer())
    {
        outfit2 = TEntityRef<IHM5Outfit>(outfit.GetEntityRef());
    }

    hitman->EquipOutfitResource(outfitVariation, outfit2, false, false);
}

void Player::EquipModel(ZActor* actor)
{
    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

    //If hitman has suit equip some other outfit so that suit doesn't stay after changing model
    if (actor->IsWearingOutfit())
    {
        hitman->EquipOutfitResource(actor->GetHMAsResourceID(), actor->GetOutfit(), false, false);
    }
    else
    {
        const OutfitKitEntity& outfitKitEntity = outfitKitEntities[1];
        const int outfitVariationIndex = 0;
        const ZRuntimeResourceID outfitVariation = outfitKitEntity.outfitVariations[0];

        ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();
        TEntityRef<ZHM5Outfit> outfit = LevelManager->GetOutfitManager().GetRawPointer()->GetOutfit(outfitKitEntity.tokenID);
        TEntityRef<IHM5Outfit> outfit2;

        if (outfit.GetRawPointer())
        {
            outfit2 = TEntityRef<IHM5Outfit>(outfit.GetEntityRef());
        }

        hitman->EquipOutfitResource(outfitVariation, outfit2, false, false);
    }

    ZResourcePtr primResourcePtr = actor->GetGeomEntityPtr()->GetResourcePtr();

    hitman->GetGeomEntityPtr()->ChangePrimitiveResourcePtr(primResourcePtr);
}

void Player::EquipModel(const std::string& resourceID)
{
    /*const OutfitKitEntity& outfitKitEntity = outfitKitEntities[1];
    const int outfitVariationIndex = 0;
    const ZRuntimeResourceID outfitVariation = outfitKitEntity.outfitVariations[0];

    ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();
    TEntityRef<ZHM5Outfit> outfit = LevelManager->GetOutfitManager().GetRawPointer()->GetOutfit(outfitKitEntity.tokenID);
    TEntityRef<IHM5Outfit> outfit2;

    if (outfit.GetRawPointer())
    {
        outfit2 = TEntityRef<IHM5Outfit>(outfit.GetEntityRef());
    }

    hitman->EquipOutfitResource(outfitVariation, outfit2, false, false);*/

    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled = ResourceUtility::CreateAndInstallDynamicResourceLibrary(dynamicResourceLibrary, resourceID, sourceResourceRuntimeResourceID);

    if (isDynamicResourceLibraryInstalled)
    {
        ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(0);
        ZGeomEntity* geomEntity = entityRef.QueryInterfacePtr<ZGeomEntity>();
        ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

        if (hitman)
        {
            const OutfitKitEntity& outfitKitEntity = outfitKitEntities[1];
            const int outfitVariationIndex = 0;
            const ZRuntimeResourceID outfitVariation = outfitKitEntity.outfitVariations[0];

            TEntityRef<ZHM5Outfit> outfit = LevelManager->GetOutfitManager().GetRawPointer()->GetOutfit(outfitKitEntity.tokenID);
            TEntityRef<IHM5Outfit> outfit2;

            if (outfit.GetRawPointer())
            {
                outfit2 = TEntityRef<IHM5Outfit>(outfit.GetEntityRef());
            }

            hitman->EquipOutfitResource(outfitVariation, outfit2, false, false);

            hitman->GetGeomEntityPtr()->ChangePrimitiveResourcePtr(geomEntity->GetResourcePtr());
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::SpawnWeapon(const ZRuntimeResourceID& runtimeResourceID, const unsigned int weaponCount, const bool addToWorld)
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled = ResourceUtility::InstallDynamicResourceLibrary(dynamicResourceLibrary, runtimeResourceID, sourceResourceRuntimeResourceID, weaponCount);

    if (isDynamicResourceLibraryInstalled)
    {
        for (unsigned int i = 0; i < dynamicResourceLibrary->GetEntityCount(); ++i)
        {
            ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(i);
            ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

            if (hitman && !addToWorld)
            {
                hitman->GetBaseInventory()->AddItemToInventory(entityRef, false, false);
            }
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::SpawnWeapon(const ZRuntimeResourceID& runtimeResourceID, ZActor* actor)
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled = ResourceUtility::InstallDynamicResourceLibrary(dynamicResourceLibrary, runtimeResourceID, sourceResourceRuntimeResourceID);

    if (isDynamicResourceLibraryInstalled)
    {
        ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(0);

        if (actor)
        {
            actor->AddItemToInventory(entityRef);
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::SpawnItem(const ZRuntimeResourceID& runtimeResourceID, const unsigned int itemCount, const bool addToWorld)
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled = false;

    if (runtimeResourceID.IsLibraryResource())
    {
        const std::string resourceID = ZRuntimeResourceID::QueryResourceID(runtimeResourceID).GetURI().ToCString();

        isDynamicResourceLibraryInstalled = ResourceUtility::CreateAndInstallDynamicResourceLibrary(dynamicResourceLibrary, resourceID, sourceResourceRuntimeResourceID, itemCount);
    }
    else
    {
        isDynamicResourceLibraryInstalled = ResourceUtility::InstallDynamicResourceLibrary(dynamicResourceLibrary, runtimeResourceID, sourceResourceRuntimeResourceID, itemCount);
    }

    if (isDynamicResourceLibraryInstalled)
    {
        for (unsigned int i = 0; i < dynamicResourceLibrary->GetEntityCount(); ++i)
        {
            ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(i);
            IHM5Item* item = entityRef.QueryInterfacePtr<IHM5Item>();
            ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

            if (hitman && !addToWorld)
            {
                hitman->GetBaseInventory()->AddItemToInventory(entityRef, false, false);
            }
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::SpawnProp()
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled = ResourceUtility::CreateAndInstallDynamicResourceLibrary(dynamicResourceLibrary, propResourceID, sourceResourceRuntimeResourceID, propCount);

    if (isDynamicResourceLibraryInstalled)
    {
        for (unsigned int i = 0; i < dynamicResourceLibrary->GetEntityCount(); ++i)
        {
            ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(i);

            entityRef.SetProperty("m_eRoomBehaviour", ZSpatialEntity::ERoomBehaviour::ROOM_DYNAMIC);
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::SpawnActor()
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    bool isDynamicResourceLibraryInstalled = ResourceUtility::CreateAndInstallDynamicResourceLibrary(dynamicResourceLibrary, actorResourceID, sourceResourceRuntimeResourceID, actorCount);

    if (isDynamicResourceLibraryInstalled)
    {
        for (unsigned int i = 0; i < dynamicResourceLibrary->GetEntityCount(); ++i)
        {
            ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(i);
            ZHM5CCProfile* hm5CCProfile = nullptr;

            if (spawnCivilianActor)
            {
                if (!strstr(actorResourceID, "male civilian"))
                {
                    SetPropertiesForCivilianActor(entityRef);
                }

                hm5CCProfile = ZHM5CCProfile::GetDefaultCCProfile(ACCP_CivilianeMale);
            }
            else
            {
                if (!strstr(actorResourceID, "male guard"))
                {
                    SetPropertiesForGuardActor(entityRef);
                }

                hm5CCProfile = ZHM5CCProfile::GetDefaultCCProfile(ACCP_GuardMale);
            }

            ZActor* actor = entityRef.QueryInterfacePtr<ZActor>();

            actor->SetCCProfile(hm5CCProfile);
            actor->SetActorName(actorName);

            if (selectedActorWeaponIndex != -1)
            {
                SpawnWeapon(fireArmKitEntities[selectedActorWeaponIndex].runtimeResourceID, actor);
            }
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Player::LoadDDSTexture(const ZRuntimeResourceID& hiResNotebookImage, ID3D11Resource** texture, ID3D11ShaderResourceView** textureView, float& width, float& height)
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

ZActor* Player::FindNearestActor()
{
    const ZSpatialEntity* hitmanSpatialEntity = LevelManager->GetHitman().GetEntityRef().QueryInterfacePtr<ZSpatialEntity>();
    TArrayRef<TEntityRef<ZActor>> actors = ActorManager->GetAliveActors();

    for (size_t i = 0; i < actors.Size(); ++i)
    {
        ZActor* actor = actors[i].GetRawPointer();
        ZEntityRef entityRef = actors[i].GetEntityRef();
        const ZSpatialEntity* actorSpatialEntity = entityRef.QueryInterfacePtr<ZSpatialEntity>();

        const SVector3 temp = hitmanSpatialEntity->GetObjectToWorldMatrix().Trans - actorSpatialEntity->GetObjectToWorldMatrix().Trans;
        const float distance = sqrt(temp.x * temp.x + temp.y * temp.y + temp.z * temp.z);

        if (distance <= 3.0f)
        {
            return actor;
        }
    }

    return nullptr;
}

void Player::SetPropertiesForCivilianActor(ZEntityRef& entityRef)
{
    const ZRuntimeResourceID aibzRuntimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID("[assembly:/ai/behaviortrees/civilian.aibt].pc_aibz");
    const ZResourcePtr aizbResourcePtr = ResourceManager->GetResourcePtr(aibzRuntimeResourceID, 0);
    TArray<ZString> statLabelList;

    statLabelList.PushBack("CivilianStatKill");

    entityRef.SetProperty("m_bZoneA", false);
    entityRef.SetProperty("m_bZoneB", false);
    entityRef.SetProperty("m_bZoneC", false);
    entityRef.SetProperty("m_bZoneD", false);
    entityRef.SetProperty("m_bZoneE", false);
    entityRef.SetProperty("m_bZoneF", false);
    entityRef.SetProperty("m_bZoneG", false);
    entityRef.SetProperty("m_bZoneH", false);
    entityRef.SetProperty("m_eActorType", EActorType::eAT_Civilian);
    entityRef.SetProperty("m_pCompiledBehaviorTree", aizbResourcePtr);
    entityRef.SetProperty("Oneliner_Mindistance", 5.f);
    entityRef.SetProperty("Oneliner_Maxdistance", 30.f);
    entityRef.SetProperty("m_fOcclusionAttenuation", -9.f);
    entityRef.SetProperty("OnelinerOcclusionCutoffLow", 3000.f);
    entityRef.SetProperty("m_fDialogueAttenuation", -6.f);
    entityRef.SetProperty("Oneliner_Attenuation", -5.f);
    entityRef.SetProperty("OnelinerOcclusionAttenuation", -12.f);
}

void Player::SetPropertiesForGuardActor(ZEntityRef& entityRef)
{
    const ZRuntimeResourceID aibzRuntimeResourceID = ZRuntimeResourceID::QueryRuntimeResourceID("[assembly:/ai/behaviortrees/guard.aibt].pc_aibz");
    const ZResourcePtr aizbResourcePtr = ResourceManager->GetResourcePtr(aibzRuntimeResourceID, 0);

    entityRef.SetProperty("m_bZoneA", true);
    entityRef.SetProperty("m_bZoneB", true);
    entityRef.SetProperty("m_bZoneC", true);
    entityRef.SetProperty("m_bZoneD", true);
    entityRef.SetProperty("m_bZoneE", true);
    entityRef.SetProperty("m_bZoneF", true);
    entityRef.SetProperty("m_bZoneG", true);
    entityRef.SetProperty("m_bZoneH", true);
    entityRef.SetProperty("m_eActorType", EActorType::eAT_Guard);
    entityRef.SetProperty("m_pCompiledBehaviorTree", aizbResourcePtr);
    entityRef.SetProperty("m_fOcclusionLowpassLow", 3000.f);
    entityRef.SetProperty("Oneliner_Attenuation", -3.f);
}

void __fastcall ZEngineAppCommon_DefaultMainLoopSequenceHook(ZEngineAppCommon* pThis, int edx)
{
    GetModInstance()->OnDefaultMainLoopSequence();

    Hooks::ZEngineAppCommon_DefaultMainLoopSequence.CallOriginalFunction(pThis);
}

ZEntityType** __fastcall ZEntityManager_ConstructUninitializedEntityHook(ZEntityManager* pThis, int edx, const ZString& sDebugName, IEntityFactory* pEntityFactory, unsigned char* pMemBlock)
{
    ZEntityType** entityType = Hooks::ZEntityManager_ConstructUninitializedEntity.CallOriginalFunction(pThis, sDebugName, pEntityFactory, pMemBlock);

    GetModInstance()->OnConstructUninitializedEntity(pEntityFactory, entityType);

    return entityType;
}

DEFINE_MOD(Player);
