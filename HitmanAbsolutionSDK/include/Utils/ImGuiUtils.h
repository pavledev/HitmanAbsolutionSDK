#pragma once

#include <string>

#include "imgui.h"
#include "imgui_internal.h"
#include <misc/cpp/imgui_stdlib.h>

#include "StringUtils.h"

namespace util
{
    /**
     * @brief Renders an ImGui input field with autocomplete functionality.
     *
     * Displays a popup with filtered options from the provided collection as the user types.
     * Items are matched based on the input text, and optionally a user-provided filter callback.
     * When an option is selected, it automatically fills the input field and triggers a callback.
     *
     * @tparam Collection             Iterable collection (e.g., TMap, std::vector) containing items.
     * @tparam GetOptionIdFn          Function or lambda that returns a unique ID for an item.
     * @tparam GetOptionNameFn        Function or lambda that returns the display name (string) for an item.
     * @tparam OnSelectFn             Callback function executed when an item is selected.
     * @tparam GetOptionValueFn       (Optional) Function or lambda returning an additional value
     *                                passed to OnSelectFn. Defaults to nullptr.
     * @tparam FilterFn               (Optional) Function or lambda used to filter out items before
     *                                text matching. If provided, only items for which the filter
     *                                returns true are considered. Defaults to nullptr.
     *
     * @param p_Id                    Unique identifier for the input and popup.
     * @param p_InputBuffer           Character buffer for the input text.
     * @param p_BufferSize            Size of the input buffer.
     * @param p_Collection            Collection of items to display and search through.
     * @param p_GetOptionIdFn         Function used to extract the item's unique ID.
     * @param p_GetOptionNameFn       Function used to extract the display name (string).
     * @param p_OnSelectFn            Callback executed when an option is selected.
     * @param p_GetOptionValueFn      (Optional) Function that retrieves the value passed to the
     *                                selection callback. If nullptr, the raw item is passed.
     * @param p_FilterFn              (Optional) Predicate function used to filter items before
     *                                substring matching. Items returning false are skipped.
     * @param p_IsCaseSensitiveSearch Whether the search should be case-sensitive.
     * @param p_PopupHeight           Height of the popup window (default: 300.f).
     *
     * @return true if the popup is open, false otherwise.
     */
    template<
        typename Collection, typename GetOptionIdFn, typename GetOptionNameFn, typename OnSelectFn, typename GetOptionValueFn = std::nullptr_t,
        typename FilterFn = std::nullptr_t>
    inline bool InputWithAutocomplete(
        const char* p_Id, char* p_InputBuffer, size_t p_BufferSize, const Collection& p_Collection, GetOptionIdFn p_GetOptionIdFn,
        GetOptionNameFn p_GetOptionNameFn, OnSelectFn p_OnSelectFn, GetOptionValueFn p_GetOptionValueFn = nullptr, FilterFn p_FilterFn = nullptr,
        bool p_IsCaseSensitiveSearch = false, float p_PopupHeight = 300.f
    )
    {
        std::string inputId = std::string(p_Id) + "_Input";

        const bool isEnterPressed = ImGui::InputText(inputId.c_str(), p_InputBuffer, p_BufferSize, ImGuiInputTextFlags_EnterReturnsTrue);
        const bool isActive = ImGui::IsItemActive();

        if (ImGui::IsItemActivated())
        {
            ImGui::OpenPopup(p_Id);
        }

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, p_PopupHeight));

        const bool isOpen = ImGui::BeginPopup(
            p_Id, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow
                      | ImGuiWindowFlags_HorizontalScrollbar
        );

        if (isOpen)
        {
            for (auto& item : p_Collection)
            {
                if constexpr (!std::is_same_v<FilterFn, std::nullptr_t>)
                {
                    if (!p_FilterFn(item))
                    {
                        continue;
                    }
                }

                const auto& optionName = p_GetOptionNameFn(item);

                if (!util::Contains(optionName, p_InputBuffer, p_IsCaseSensitiveSearch))
                {
                    continue;
                }

                const auto& optionId = p_GetOptionIdFn(item);

                ImGui::PushID(&optionId);

                const bool selected = ImGui::Selectable(optionName.c_str());

                ImGui::PopID();

                if (selected)
                {
                    ImGui::ClearActiveID();

                    strcpy_s(p_InputBuffer, p_BufferSize, optionName.c_str());

                    if constexpr (!std::is_same_v<GetOptionValueFn, std::nullptr_t>)
                    {
                        auto optionValue = p_GetOptionValueFn(item);

                        p_OnSelectFn(optionId, optionName, optionValue);
                    }
                    else
                    {
                        p_OnSelectFn(optionId, optionName, item);
                    }
                }
            }

            if (isEnterPressed || (!isActive && !ImGui::IsWindowFocused()))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        return isOpen;
    }

    /**
     * @brief Renders an ImGui input field with autocomplete functionality.
     *
     * Displays a popup with filtered options from the provided collection as the user types.
     * Items are matched based on the input text, and optionally a user-provided filter callback.
     * When an option is selected, it automatically fills the input field and triggers a callback.
     *
     * @tparam Collection             Iterable collection (e.g., TMap, std::vector) containing items.
     * @tparam GetOptionIdFn          Function or lambda that returns a unique ID for an item.
     * @tparam GetOptionNameFn        Function or lambda that returns the display name (string) for an item.
     * @tparam OnSelectFn             Callback function executed when an item is selected.
     * @tparam GetOptionValueFn       (Optional) Function or lambda returning an additional value
     *                                passed to OnSelectFn. Defaults to nullptr.
     * @tparam FilterFn               (Optional) Function or lambda used to filter out items before
     *                                text matching. If provided, only items for which the filter
     *                                returns true are considered. Defaults to nullptr.
     *
     * @param p_Id                    Unique identifier for the input and popup.
     * @param p_InputBuffer           Character buffer for the input text.
     * @param p_Collection            Collection of items to display and search through.
     * @param p_GetOptionIdFn         Function used to extract the item's unique ID.
     * @param p_GetOptionNameFn       Function used to extract the display name (string).
     * @param p_OnSelectFn            Callback executed when an option is selected.
     * @param p_GetOptionValueFn      (Optional) Function that retrieves the value passed to the
     *                                selection callback. If nullptr, the raw item is passed.
     * @param p_FilterFn              (Optional) Predicate function used to filter items before
     *                                substring matching. Items returning false are skipped.
     * @param p_IsCaseSensitiveSearch Whether the search should be case-sensitive.
     * @param p_PopupHeight           Height of the popup window (default: 300.f).
     *
     * @return true if the popup is open, false otherwise.
     */
    template<
        typename Collection, typename GetOptionIdFn, typename GetOptionNameFn, typename OnSelectFn, typename GetOptionValueFn = std::nullptr_t,
        typename FilterFn = std::nullptr_t>
    static bool InputWithAutocomplete(
        const char* p_Id, std::string& p_InputBuffer, const Collection& p_Collection, GetOptionIdFn p_GetOptionIdFn,
        GetOptionNameFn p_GetOptionNameFn, OnSelectFn p_OnSelectFn, GetOptionValueFn p_GetOptionValueFn = nullptr, FilterFn p_FilterFn = nullptr,
        bool p_IsCaseSensitiveSearch = false, float p_PopupHeight = 300.f
    )
    {
        std::string inputId = std::string(p_Id) + "_Input";

        const bool isEnterPressed = ImGui::InputText(inputId.c_str(), &p_InputBuffer, ImGuiInputTextFlags_EnterReturnsTrue);
        const bool isActive = ImGui::IsItemActive();

        if (ImGui::IsItemActivated())
        {
            ImGui::OpenPopup(p_Id);
        }

        ImGui::SetNextWindowPos(ImVec2(ImGui::GetItemRectMin().x, ImGui::GetItemRectMax().y));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetItemRectSize().x, p_PopupHeight));

        const bool isOpen = ImGui::BeginPopup(
            p_Id, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_ChildWindow
                      | ImGuiWindowFlags_HorizontalScrollbar
        );

        if (isOpen)
        {
            for (auto& item : p_Collection)
            {
                if constexpr (!std::is_same_v<FilterFn, std::nullptr_t>)
                {
                    if (!p_FilterFn(item))
                    {
                        continue;
                    }
                }

                const auto& optionName = p_GetOptionNameFn(item);

                if (!util::Contains(optionName, p_InputBuffer, p_IsCaseSensitiveSearch))
                {
                    continue;
                }

                const auto& optionId = p_GetOptionIdFn(item);

                ImGui::PushID(&optionId);

                const bool selected = ImGui::Selectable(optionName.c_str());

                ImGui::PopID();

                if (selected)
                {
                    ImGui::ClearActiveID();

                    p_InputBuffer = optionName;

                    if constexpr (!std::is_same_v<GetOptionValueFn, std::nullptr_t>)
                    {
                        auto optionValue = p_GetOptionValueFn(item);

                        p_OnSelectFn(optionId, optionName, optionValue);
                    }
                    else
                    {
                        p_OnSelectFn(optionId, optionName, item);
                    }
                }
            }

            if (isEnterPressed || (!isActive && !ImGui::IsWindowFocused()))
            {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        return isOpen;
    }
}
