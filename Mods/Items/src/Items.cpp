#include <IconsMaterialDesign.h>

#include <Glacier/ZAction.h>
#include <Glacier/ZItem.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/ZActor.h>

#include <Items.h>

Items::Items()
{
    isOpen = false;
    selectedItemIndex = -1;
}

void Items::OnDrawMenu(IImGuiRenderer* p_Renderer)
{
    if (ImGui::Button(ICON_MD_BUILD " Items"))
    {
        isOpen = !isOpen;
    }
}

void Items::OnDrawUI(IImGuiRenderer* p_Renderer, bool p_HasFocus)
{
    if (!p_HasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(p_Renderer->GetBlackFont());
    ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_TOKEN " Items", &isOpen, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(p_Renderer->GetRegularFont());

    if (isWindowVisible)
    {
        ImGui::BeginChild("left pane", ImVec2(600, 500), false, ImGuiWindowFlags_HorizontalScrollbar);

        static char itemName[256]{ "" };

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Item Name");
        ImGui::SameLine();
        ImGui::InputText("##ItemName", itemName, sizeof(itemName));

        unsigned int index = 0;

        for (auto it = Globals::HM5ActionManager->m_Actions.Begin(); it != Globals::HM5ActionManager->m_Actions.End(); ++it)
        {
            EActionType actionType = it.Node().m_data.m_pNodeAction->m_eActionType;

            if (actionType != EActionType::AT_PICKUP)
            {
                ++index;

                continue;
            }

            TEntityRef<IEntity> object = it.Node().m_data.m_pNodeAction->m_Object;
            IHM5Item* item = object.m_entityRef.QueryInterfacePtr<IHM5Item>();
            std::string itemName2 = std::format("{} ({})", item->GetItemName().ToCString(), index);

            if (!util::Contains(itemName2, itemName, false))
            {
                ++index;

                continue;
            }

            bool isActorSelected = selectedItemIndex == index;

            if (ImGui::Selectable(itemName2.c_str(), &isActorSelected))
            {
                selectedItemIndex = index;
            }

            ++index;
        }

        ImGui::EndChild();

        ImGui::SameLine();
        ImGui::BeginChild("ItemView", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()));

        if (selectedItemIndex != -1)
        {
            if (ImGui::Button("Add item to inventory"))
            {
                TEntityRef<IEntity> object = Globals::HM5ActionManager->m_Actions[selectedItemIndex].m_pNodeAction->m_Object;
                ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

                if (hitman)
                {
                    hitman->m_pBaseInventory->AddItemToInventory(object.m_entityRef, false, false);
                }
            }

            if (ImGui::Button("Teleport item to hitman"))
            {
                TEntityRef<IEntity> object = Globals::HM5ActionManager->m_Actions[selectedItemIndex].m_pNodeAction->m_Object;
                IHM5Item* item = object.m_entityRef.QueryInterfacePtr<IHM5Item>();
                ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntityPtr();
                    ZSpatialEntity* itemSpatialEntity = item->GetSpatialEntity().m_pInterfaceRef;

                    itemSpatialEntity->SetWorldPosition(hitmanSpatialEntity->GetWorldPosition());
                }
            }
        }

        ImGui::EndChild();

        if (ImGui::Button("Teleport all items to hitman"))
        {
            for (auto it = Globals::HM5ActionManager->m_Actions.Begin(); it != Globals::HM5ActionManager->m_Actions.End(); ++it)
            {
                const EActionType actionType = it.Node().m_data.m_pNodeAction->m_eActionType;

                if (actionType != EActionType::AT_PICKUP)
                {
                    continue;
                }

                const TEntityRef<IEntity> object = it.Node().m_data.m_pNodeAction->m_Object;
                const IHM5Item* item = object.m_entityRef.QueryInterfacePtr<IHM5Item>();
                const ZEntityRef ownerEntityRef = item->GetOwner();

                if (ownerEntityRef.m_pEntityTypePtrPtr)
                {
                    continue;
                }

                ZHitman5* hitman = Globals::LevelManager->m_rHitman.m_pInterfaceRef;

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntityPtr();
                    ZSpatialEntity* itemSpatialEntity = item->GetSpatialEntity().m_pInterfaceRef;

                    itemSpatialEntity->SetWorldPosition(hitmanSpatialEntity->GetWorldPosition());
                }
            }
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

DEFINE_HMASDK_MOD(Items);
