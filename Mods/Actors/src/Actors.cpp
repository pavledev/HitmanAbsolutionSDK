#include <imgui_internal.h>

#include <rapidjson/istreamwrapper.h>
#include <rapidjson/document.h>

#include <IconsMaterialDesign.h>

#include <Glacier/ZActor.h>
#include <Glacier/ZHitman5.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/ZGameLoopManager.h>
#include <Glacier/ZDynamicResourceLibrary.h>

#include <Actors.h>
#include <Utils/ResourceUtils.h>
#include <Hooks.h>
#include <Renderer/DirectXRenderer.h>

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

    Globals::GameLoopManager->UnregisterForFrameUpdate(delegate);

    /*for (size_t i = 0; i < dynamicResourceLibraries.size(); ++i)
    {
        util::FreeObject(dynamicResourceLibraries[i]);
    }*/
}

void Actors::OnEngineInitialized()
{
    const ZMemberDelegate<Actors, void(const SGameUpdateEvent&)> delegate(this, &Actors::OnFrameUpdate);

    Globals::GameLoopManager->RegisterForFrameUpdate(delegate, 1);
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
            *Globals::GodMode = static_cast<int32_t>(isGodModeEnabled);
        }

        static char actorName[256]{ "" };

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Actor Name");
        ImGui::SameLine();
        ImGui::InputText("##ActorName", actorName, sizeof(actorName));

        for (size_t i = 0; i < Globals::ActorManager->m_aliveActors.Size(); ++i)
        {
            ZActor* actor = Globals::ActorManager->m_aliveActors[i].m_pInterfaceRef;
            const ZString& actorName2 = actor->m_sActorName;

            if (!util::Contains(actorName2.ToCString(), actorName, false))
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
            ZActor* actor = Globals::ActorManager->m_aliveActors[selectedActorIndex].m_pInterfaceRef;
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
                ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().m_pInterfaceRef;
                    ZSpatialEntity* actorSpatialEntity = actor->GetSpatialEntity().m_pInterfaceRef;

                    actorSpatialEntity->SetWorldPosition(hitmanSpatialEntity->GetWorldPosition());
                }
            }

            if (ImGui::Button("Teleport hitman to actor"))
            {
                ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().m_pInterfaceRef;
                    ZSpatialEntity* actorSpatialEntity = actor->GetSpatialEntity().m_pInterfaceRef;

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
            const bool isInputTextEnterPressed =
                ImGui::InputText("##WeaponName", actorWeaponName, sizeof(actorWeaponName), ImGuiInputTextFlags_EnterReturnsTrue);
            const bool isInputTextActive = ImGui::IsItemActive();

            if (ImGui::IsItemActivated())
            {
                ImGui::OpenPopup("##Popup");
            }

            ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
            ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, 300));

            if (ImGui::BeginPopup(
                    "##Popup", ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow
                ))
            {
                for (size_t i = 0; i < fireArmKitEntities.size(); ++i)
                {
                    if (!util::Contains(fireArmKitEntities[i].title, actorWeaponName, false))
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
            for (unsigned int i = 0; i < Globals::ActorManager->m_aliveActors.Size(); i++)
            {
                ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntityPtr();
                    ZSpatialEntity* actorSpatialEntity = Globals::ActorManager->m_aliveActors[i].m_pInterfaceRef->GetSpatialEntityPtr();

                    hitmanSpatialEntity->SetWorldPosition(actorSpatialEntity->GetWorldPosition());
                }
            }
        }

        if (ImGui::Button("Kill all actors"))
        {
            for (unsigned int i = 0; i < Globals::ActorManager->m_aliveActors.Size(); i++)
            {
                Globals::ActorManager->m_aliveActors[i].m_pInterfaceRef->KillActor(EActorDeathType::eADT_UNDEFINED, true);
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
        for (size_t i = 0; i < Globals::ActorManager->m_aliveActors.Size(); ++i)
        {
            float4 worldPosition = Globals::ActorManager->m_aliveActors[i].m_pInterfaceRef->GetWorldPosition();
            SVector2 screenPosition;

            if (sdk.GetDirectXRenderer()->WorldToScreen(SVector3(worldPosition.x, worldPosition.y, worldPosition.z + 2.05f), screenPosition))
            {
                sdk.GetDirectXRenderer()->DrawText2D(
                    Globals::ActorManager->m_aliveActors[i].m_pInterfaceRef->m_sActorName, screenPosition, SVector4(1.f, 0.f, 0.f, 1.f), 0.f, 0.5f
                );
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
    /*ZDynamicResourceLibrary* dynamicResourceLibrary;
    ZRuntimeResourceID sourceResourceRuntimeResourceID;
    bool isDynamicResourceLibraryInstalled =
        util::CreateAndInstallDynamicResourceLibrary(dynamicResourceLibrary, resourceID, sourceResourceRuntimeResourceID);

    if (isDynamicResourceLibraryInstalled)
    {
        ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(0);
        ZGeomEntity* geomEntity = entityRef.QueryInterfacePtr<ZGeomEntity>();

        if (actor)
        {
            actor->GetGeomEntityPtr()->ChangePrimitiveResourcePtr(geomEntity->GetResourcePtr());
        }

        dynamicResourceLibraries.push_back(dynamicResourceLibrary);
    }*/
}

void Actors::SpawnWeapon(const ZRuntimeResourceID& runtimeResourceID)
{
    ZDynamicResourceLibrary* dynamicResourceLibrary;
    ZRuntimeResourceID sourceResourceRuntimeResourceID;
    bool isDynamicResourceLibraryInstalled =
        util::InstallDynamicResourceLibrary(runtimeResourceID, dynamicResourceLibrary, sourceResourceRuntimeResourceID);

    if (isDynamicResourceLibraryInstalled)
    {
        ZEntityRef entityRef = dynamicResourceLibrary->GetEntity(0);
        ZActor* actor = Globals::ActorManager->m_aliveActors[selectedActorIndex].m_pInterfaceRef;

        if (actor)
        {
            actor->m_runtimeInventory[0] = entityRef;
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
