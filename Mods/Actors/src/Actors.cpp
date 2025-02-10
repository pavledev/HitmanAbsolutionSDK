#include <imgui_internal.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>

#include <IconsMaterialDesign.h>

#include <Glacier/Actor/ZActorManager.h>
#include <Glacier/Player/ZHitman5.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/ZGameLoopManager.h>

#include <Actors.h>
#include <Utility/ResourceUtility.h>
#include <Utility/MemoryUtility.h>
#include <Hooks.h>

Actors::Actors()
{
    isOpen = false;
    selectedActorIndex = -1;
    renderActorNames = false;
    isGodModeEnabled = false;
    spawnWeapon = false;
    selectedActorWeaponIndex = -1;
}

Actors::~Actors()
{
    const ZMemberDelegate<Actors, void(const SGameUpdateEvent&)> delegate(this, &Actors::OnFrameUpdate);

    GameLoopManager->UnregisterForFrameUpdate(delegate);

    for (size_t i = 0; i < dynamicResourceLibraries.size(); ++i)
    {
        FreeObject(dynamicResourceLibraries[i]);
    }
}

void Actors::Initialize()
{
    ModInterface::Initialize();

    godMode = reinterpret_cast<int*>(BaseAddress + 0xD4D91C);
}

void Actors::OnEngineInitialized()
{
    const ZMemberDelegate<Actors, void(const SGameUpdateEvent&)> delegate(this, &Actors::OnFrameUpdate);

    GameLoopManager->RegisterForFrameUpdate(delegate, 1);
}

void Actors::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_MAN " Actors"))
    {
        isOpen = !isOpen;
    }
}

void Actors::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_TOKEN " Actors", &isOpen, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        if (fireArmKitEntities.size() == 0)
        {
            GetWeapons();
        }

        ImGui::BeginChild("left pane", ImVec2(600, 500), false, ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::Checkbox("Render Actor Names", &renderActorNames);

        if (ImGui::Checkbox("God mode", &isGodModeEnabled))
        {
            *godMode = static_cast<int>(isGodModeEnabled);
        }

        static char actorName[256]{ "" };

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Actor Name");
        ImGui::SameLine();
        ImGui::InputText("##ActorName", actorName, sizeof(actorName));

        TArrayRef<TEntityRef<ZActor>> actors = ActorManager->GetAliveActors();

        for (size_t i = 0; i < actors.Size(); ++i)
        {
            ZActor* actor = actors[i].GetRawPointer();
            const ZString& actorName2 = actor->GetActorName();

            if (!strstr(actorName2.ToCString(), actorName))
            {
                continue;
            }

            bool isActorSelected = selectedActorIndex == i;

            if (ImGui::Selectable(actorName2.ToCString(), &isActorSelected))
            {
                selectedActorIndex = i;
            }
        }

        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("ItemView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        if (selectedActorIndex != -1)
        {
            ZActor* actor = actors[selectedActorIndex].GetRawPointer();
            static char resourceID[512]{ "" };

            ImGui::AlignTextToFramePadding();
            ImGui::Text("Resource ID");
            ImGui::SameLine();
            ImGui::InputText("##ResourceID", resourceID, sizeof(resourceID), ImGuiInputTextFlags_EnterReturnsTrue);

            if (ImGui::Button("Equip Model"))
            {
                EquipModel(actor, resourceID);
            }

            ImGui::Separator();

            if (ImGui::Button("Teleport actor to hitman"))
            {
                ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().GetRawPointer();
                    ZSpatialEntity* actorSpatialEntity = actor->GetSpatialEntity().GetRawPointer();

                    actorSpatialEntity->SetWorldPosition(hitmanSpatialEntity->GetWorldPosition());
                }
            }

            if (ImGui::Button("Teleport hitman to actor"))
            {
                ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().GetRawPointer();
                    ZSpatialEntity* actorSpatialEntity = actor->GetSpatialEntity().GetRawPointer();

                    hitmanSpatialEntity->SetWorldPosition(actorSpatialEntity->GetWorldPosition());
                }
            }

            if (ImGui::Button("Kill Actor"))
            {
                if (actor)
                {
                    actor->KillActor(EActorDeathType::eADT_UNDEFINED, true);
                }
            }

            ImGui::Separator();

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

            ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

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
                        spawnWeapon = true;
                    }
                }

                if (isInputTextEnterPressed || (!isInputTextActive && !ImGui::IsWindowFocused()))
                {
                    ImGui::CloseCurrentPopup();
                }

                ImGui::EndPopup();
            }
        }

        ImGui::EndChild();

        if (ImGui::Button("Teleport all actors to hitman"))
        {
            for (unsigned int i = 0; i < actors.Size(); i++)
            {
                ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().GetRawPointer();
                    ZSpatialEntity* actorSpatialEntity = actors[i]->GetSpatialEntity().GetRawPointer();

                    hitmanSpatialEntity->SetWorldPosition(actorSpatialEntity->GetWorldPosition());
                }
            }
        }

        if (ImGui::Button("Kill all actors"))
        {
            for (unsigned int i = 0; i < actors.Size(); i++)
            {
                actors[i].GetRawPointer()->KillActor(EActorDeathType::eADT_UNDEFINED, true);
            }
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

void Actors::OnDraw3D()
{
    SDK& sdk = SDK::GetInstance();

    if (renderActorNames)
    {
        TArrayRef<TEntityRef<ZActor>> actors = ActorManager->GetAliveActors();

        for (size_t i = 0; i < actors.Size(); ++i)
        {
            ZActor* actor = actors[i].GetRawPointer();
            float4 worldPosition = actor->GetWorldPosition();
            SVector2 screenPosition;

            if (sdk.GetDirectXRenderer()->WorldToScreen(SVector3(worldPosition.x, worldPosition.y, worldPosition.z + 2.05f), screenPosition))
            {
                sdk.GetDirectXRenderer()->DrawText2D(actor->GetActorName(), screenPosition, SVector4(1.f, 0.f, 0.f, 1.f), 0.f, 0.5f);
            }
        }
    }
}

void Actors::OnFrameUpdate(const SGameUpdateEvent& updateEvent)
{
    if (spawnWeapon)
    {
        SpawnWeapon(fireArmKitEntities[selectedActorWeaponIndex].runtimeResourceID);

        spawnWeapon = false;
    }
}

void Actors::EquipModel(ZActor* actor, const std::string& resourceID)
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    ZRuntimeResourceID sourceResourceRuntimeResourceID;
    bool isDynamicResourceLibraryInstalled = ResourceUtility::CreateAndInstallDynamicResourceLibrary(dynamicResourceLibrary, resourceID, sourceResourceRuntimeResourceID);

    if (isDynamicResourceLibraryInstalled)
    {
        ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(0);
        ZGeomEntity* geomEntity = entityRef.QueryInterfacePtr<ZGeomEntity>();

        if (actor)
        {
            actor->GetGeomEntityPtr()->ChangePrimitiveResourcePtr(geomEntity->GetResourcePtr());
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Actors::SpawnWeapon(const ZRuntimeResourceID& runtimeResourceID)
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    ZRuntimeResourceID sourceResourceRuntimeResourceID;
    bool isDynamicResourceLibraryInstalled = ResourceUtility::InstallDynamicResourceLibrary(dynamicResourceLibrary, runtimeResourceID, sourceResourceRuntimeResourceID);

    if (isDynamicResourceLibraryInstalled)
    {
        ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(0);
        TArrayRef<TEntityRef<ZActor>> actors = ActorManager->GetAliveActors();
        ZActor* actor = actors[selectedActorIndex].GetRawPointer();
        TArray<TEntityRef<IHM5Item>>& runtimeInventory = actor->GetRuntimeInventory();

        if (actor)
        {
            runtimeInventory[0] = entityRef;
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }
}

void Actors::GetWeapons()
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
        fireArmKitEntity.tokenID = STokenID(object["hash"].GetUint());
        fireArmKitEntity.runtimeResourceID = object["runtimeResourceID"].GetUint64();
    }
}

DEFINE_MOD(Actors);
