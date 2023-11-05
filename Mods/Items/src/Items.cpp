#include <IconsMaterialDesign.h>

#include <Glacier/Action/ZHM5ActionManager.h>
#include <Glacier/Item/IHM5Item.h>
#include <Glacier/ZLevelManager.h>
#include <Glacier/Actor/ZActor.h>

#include <Items.h>

Items::Items()
{
    isOpen = false;
    selectedItemIndex = -1;
}

void Items::OnDrawMenu()
{
    if (ImGui::Button(ICON_MD_BUILD " Items"))
    {
        isOpen = !isOpen;
    }
}

void Items::OnDrawUI(const bool hasFocus)
{
    if (!hasFocus || !isOpen)
    {
        return;
    }

    ImGui::PushFont(SDK::GetInstance().GetBoldFont());
    ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiCond_FirstUseEver);

    const bool isWindowVisible = ImGui::Begin(ICON_MD_TOKEN " Items", &isOpen, ImGuiWindowFlags_NoScrollbar);

    ImGui::PushFont(SDK::GetInstance().GetRegularFont());

    if (isWindowVisible)
    {
        ImGui::BeginChild("left pane", ImVec2(600, 500), false, ImGuiWindowFlags_HorizontalScrollbar);

        static char itemName[256]{ "" };

        ImGui::AlignTextToFramePadding();
        ImGui::Text("Item Name");
        ImGui::SameLine();
        ImGui::InputText("##ItemName", itemName, sizeof(itemName));

        TSList<ZHM5ActionManager::SActionTreeEntry>& actions = HM5ActionManager->GetActions();
        unsigned int index = 0;

        for (auto it = actions.Begin(); it != actions.End(); ++it)
        {
            EActionType actionType = it.Node().m_data.m_pNodeAction->GetActionType();

            if (actionType != EActionType::AT_PICKUP)
            {
                ++index;

                continue;
            }

            TEntityRef<IEntity> object = it.Node().m_data.m_pNodeAction->GetActionObject();
            IHM5Item* item = object.GetEntityRef().QueryInterfacePtr<IHM5Item>();
            std::string itemName2 = std::format("{} ({})", item->GetItemName().ToCString(), index);

            if (!strstr(itemName2.c_str(), itemName))
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
                TEntityRef<IEntity> object = actions[selectedItemIndex].m_pNodeAction->GetActionObject();
                ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

                if (hitman)
                {
                    hitman->GetBaseInventory()->AddItemToInventory(object.GetEntityRef(), false, false);
                }
            }

            if (ImGui::Button("Teleport item to hitman"))
            {
                TEntityRef<IEntity> object = actions[selectedItemIndex].m_pNodeAction->GetActionObject();
                IHM5Item* item = object.GetEntityRef().QueryInterfacePtr<IHM5Item>();
                ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().GetRawPointer();
                    ZSpatialEntity* itemSpatialEntity = item->GetSpatialEntity().GetRawPointer();

                    itemSpatialEntity->SetWorldPosition(hitmanSpatialEntity->GetWorldPosition());
                }
            }
        }

        ImGui::EndChild();

        if (ImGui::Button("Teleport all items to hitman"))
        {
            for (auto it = actions.Begin(); it != actions.End(); ++it)
            {
                const EActionType actionType = it.Node().m_data.m_pNodeAction->GetActionType();

                if (actionType != EActionType::AT_PICKUP)
                {
                    continue;
                }

                const TEntityRef<IEntity> object = it.Node().m_data.m_pNodeAction->GetActionObject();
                const IHM5Item* item = object.GetEntityRef().QueryInterfacePtr<IHM5Item>();
                const ZEntityRef ownerEntityRef = item->GetOwner();

                if (ownerEntityRef.GetEntityTypePtrPtr())
                {
                    continue;
                }

                ZHitman5* hitman = LevelManager->GetHitman().GetRawPointer();

                if (hitman)
                {
                    ZSpatialEntity* hitmanSpatialEntity = hitman->GetSpatialEntity().GetRawPointer();
                    ZSpatialEntity* itemSpatialEntity = item->GetSpatialEntity().GetRawPointer();

                    itemSpatialEntity->SetWorldPosition(hitmanSpatialEntity->GetWorldPosition());
                }
            }
        }
    }

    ImGui::PopFont();
    ImGui::End();
    ImGui::PopFont();
}

DEFINE_MOD(Items);
