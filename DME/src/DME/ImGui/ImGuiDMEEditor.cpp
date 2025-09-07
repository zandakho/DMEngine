#include "dmepch.h"

#include "ImGuiDMEEditor.h"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui.h"
#include "imgui_internal.h"


namespace DME
{
    namespace ImGuiDMEEditor
    {
        bool Checkbox(const char* label, bool* v)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            const ImGuiID id = window->GetID(label);
            const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

            const float square_sz = ImGui::GetFrameHeight();
            const ImVec2 pos = window->DC.CursorPos;
            const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
            ImGui::ItemSize(total_bb, style.FramePadding.y);
            const bool is_visible = ImGui::ItemAdd(total_bb, id);
            const bool is_multi_select = (g.LastItemData.ItemFlags & ImGuiItemFlags_IsMultiSelect) != 0;
            if (!is_visible)
                if (!is_multi_select || !g.BoxSelectState.UnclipMode || !g.BoxSelectState.UnclipRect.Overlaps(total_bb))
                {
                    IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
                    return false;
                }

            bool checked = *v;
            if (is_multi_select)
                ImGui::MultiSelectItemHeader(id, &checked, NULL);

            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);

            if (is_multi_select)
                ImGui::MultiSelectItemFooter(id, &checked, &pressed);
            else if (pressed)
                checked = !checked;

            if (*v != checked)
            {
                *v = checked;
                pressed = true;
                ImGui::MarkItemEdited(id);
            }

            const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
            const bool mixed_value = (g.LastItemData.ItemFlags & ImGuiItemFlags_MixedValue) != 0;
            if (is_visible)
            {
                ImGui::RenderNavCursor(total_bb, id);
                ImGui::RenderFrame(check_bb.Min, check_bb.Max, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), true, style.FrameRounding);
                ImU32 check_col = ImGui::GetColorU32(ImGuiCol_CheckMark);
                if (mixed_value)
                {
                    ImVec2 pad(ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)), ImMax(1.0f, IM_TRUNC(square_sz / 3.6f)));
                    window->DrawList->AddRectFilled(check_bb.Min + pad, check_bb.Max - pad, check_col, style.FrameRounding);
                }
                else if (*v)
                {
                    const float pad = ImMax(1.0f, IM_TRUNC(square_sz / 6.0f));
                    ImGui::RenderCheckMark(window->DrawList, check_bb.Min + ImVec2(pad, pad), check_col, square_sz - pad * 2.0f);
                }
            }
            const ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
            if (g.LogEnabled)
                ImGui::LogRenderedText(&label_pos, mixed_value ? "[~]" : *v ? "[x]" : "[ ]");
            if (is_visible && label_size.x > 0.0f)
                ImGui::RenderText(label_pos, label);

            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (*v ? ImGuiItemStatusFlags_Checked : 0));
            return pressed;
        }
        bool CheckboxAsRadio(const char* label, bool* value, unsigned long long* iconTexture)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            const ImGuiID id = window->GetID(label);

            // Calculate sizes
            const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
            const float circle_radius = 8.0f;
            const ImVec2 circle_size = ImVec2(circle_radius * 2, circle_radius * 2);
            const ImVec2 icon_size = iconTexture ? ImVec2(16.0f, 16.0f) : ImVec2(0.0f, 0.0f);

            // Spacing
            const float circle_icon_spacing = iconTexture ? 8.0f : 0.0f;
            const float icon_text_spacing = 8.0f;

            // Total width calculation
            const float total_width = circle_size.x + circle_icon_spacing + icon_size.x +
                icon_text_spacing + label_size.x;
            const float total_height = ImMax(ImMax(circle_size.y, icon_size.y), label_size.y);

            const ImVec2 total_size = ImVec2(total_width, total_height);

            // Create bounding box
            const ImRect total_bb(window->DC.CursorPos, window->DC.CursorPos + total_size);
            const ImVec2 circle_center = ImVec2(
                total_bb.Min.x + circle_radius,
                total_bb.Min.y + total_height * 0.5f
            );

            ImGui::ItemSize(total_bb, style.FramePadding.y);
            if (!ImGui::ItemAdd(total_bb, id))
                return false;

            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
            if (pressed)
            {
                *value = !(*value);
                ImGui::MarkItemEdited(id);
            }

            // Colors
            const ImU32 circle_color = *value ?
                ImGui::GetColorU32(ImVec4(0.0f, 0.47f, 1.0f, 1.0f)) : // Blue when checked
                ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f));   // Gray when unchecked

            const ImU32 circle_border = ImGui::GetColorU32(ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

            // Background on hover (blue highlight like in game engines)
            if (hovered)
            {
                window->DrawList->AddRectFilled(
                    total_bb.Min,
                    total_bb.Max,
                    ImGui::GetColorU32(ImVec4(0.1f, 0.3f, 0.6f, 0.3f)), // Semi-transparent blue
                    4.0f
                );
            }

            // Draw circle - просто кружок без всяких галочек
            window->DrawList->AddCircleFilled(circle_center, circle_radius, circle_color);
            window->DrawList->AddCircle(circle_center, circle_radius, circle_border, 0, 1.5f);

            // Draw icon if provided
            if (iconTexture)
            {
                const ImVec2 icon_pos = ImVec2(
                    total_bb.Min.x + circle_size.x + circle_icon_spacing,
                    total_bb.Min.y + (total_height - icon_size.y) * 0.5f
                );

                // Apply tint based on state
                ImU32 icon_color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                if (!*value)
                {
                    icon_color = ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 0.8f)); // Dimmed when unchecked
                }

                window->DrawList->AddImage(iconTexture, icon_pos, icon_pos + icon_size,
                    ImVec2(0, 0), ImVec2(1, 1), icon_color);
            }

            // Draw label
            if (label_size.x > 0.0f)
            {
                const float text_offset_x = circle_size.x + circle_icon_spacing + icon_size.x + icon_text_spacing;
                const ImVec2 text_pos = ImVec2(
                    total_bb.Min.x + text_offset_x,
                    total_bb.Min.y + (total_height - label_size.y) * 0.5f
                );

                // Text color based on state
                const ImU32 text_color = *value ?
                    ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) : // White when checked
                    ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));  // Light gray when unchecked

                window->DrawList->AddText(text_pos, text_color, label);
            }

            return pressed;
        }
        bool RadioButton(const char* label, bool active)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            const ImGuiID id = window->GetID(label);
            const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

            const float square_sz = ImGui::GetFrameHeight();
            const ImVec2 pos = window->DC.CursorPos;
            const ImRect check_bb(pos, pos + ImVec2(square_sz, square_sz));
            const ImRect total_bb(pos, pos + ImVec2(square_sz + (label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f), label_size.y + style.FramePadding.y * 2.0f));
            ImGui::ItemSize(total_bb, style.FramePadding.y);
            if (!ImGui::ItemAdd(total_bb, id))
                return false;

            ImVec2 center = check_bb.GetCenter();
            center.x = IM_ROUND(center.x);
            center.y = IM_ROUND(center.y);
            const float radius = (square_sz - 1.0f) * 0.5f;

            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(total_bb, id, &hovered, &held);
            if (pressed)
                ImGui::MarkItemEdited(id);

            ImGui::RenderNavCursor(total_bb, id);
            const int num_segment = window->DrawList->_CalcCircleAutoSegmentCount(radius);
            window->DrawList->AddCircleFilled(center, radius, ImGui::GetColorU32((held && hovered) ? ImGuiCol_FrameBgActive : hovered ? ImGuiCol_FrameBgHovered : ImGuiCol_FrameBg), num_segment);
            if (active)
            {
                const float pad = ImMax(1.0f, IM_TRUNC(square_sz / 6.0f));
                window->DrawList->AddCircleFilled(center, radius - pad, ImGui::GetColorU32(ImGuiCol_CheckMark));
            }

            if (style.FrameBorderSize > 0.0f)
            {
                window->DrawList->AddCircle(center + ImVec2(1, 1), radius, ImGui::GetColorU32(ImGuiCol_BorderShadow), num_segment, style.FrameBorderSize);
                window->DrawList->AddCircle(center, radius, ImGui::GetColorU32(ImGuiCol_Border), num_segment, style.FrameBorderSize);
            }

            ImVec2 label_pos = ImVec2(check_bb.Max.x + style.ItemInnerSpacing.x, check_bb.Min.y + style.FramePadding.y);
            if (g.LogEnabled)
                ImGui::LogRenderedText(&label_pos, active ? "(x)" : "( )");
            if (label_size.x > 0.0f)
                ImGui::RenderText(label_pos, label);

            IMGUI_TEST_ENGINE_ITEM_INFO(id, label, g.LastItemData.StatusFlags);
            return pressed;
        }
        
        static bool IsRootOfOpenMenuSet()
        {
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = g.CurrentWindow;
            if ((g.OpenPopupStack.Size <= g.BeginPopupStack.Size) || (window->Flags & ImGuiWindowFlags_ChildMenu))
                return false;

            // Initially we used 'upper_popup->OpenParentId == window->IDStack.back()' to differentiate multiple menu sets from each others
            // (e.g. inside menu bar vs loose menu items) based on parent ID.
            // This would however prevent the use of e.g. PushID() user code submitting menus.
            // Previously this worked between popup and a first child menu because the first child menu always had the _ChildWindow flag,
            // making hovering on parent popup possible while first child menu was focused - but this was generally a bug with other side effects.
            // Instead we don't treat Popup specifically (in order to consistently support menu features in them), maybe the first child menu of a Popup
            // doesn't have the _ChildWindow flag, and we rely on this IsRootOfOpenMenuSet() check to allow hovering between root window/popup and first child menu.
            // In the end, lack of ID check made it so we could no longer differentiate between separate menu sets. To compensate for that, we at least check parent window nav layer.
            // This fixes the most common case of menu opening on hover when moving between window content and menu bar. Multiple different menu sets in same nav layer would still
            // open on hover, but that should be a lesser problem, because if such menus are close in proximity in window content then it won't feel weird and if they are far apart
            // it likely won't be a problem anyone runs into.
            const ImGuiPopupData* upper_popup = &g.OpenPopupStack[g.BeginPopupStack.Size];
            if (window->DC.NavLayerCurrent != upper_popup->ParentNavLayer)
                return false;
            return upper_popup->Window && (upper_popup->Window->Flags & ImGuiWindowFlags_ChildMenu) && ImGui::IsWindowChildOf(upper_popup->Window, window, true, false);
        }

        bool MenuItemEx(const char* label, const char* icon, const char* shortcut, bool selected, bool enabled)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            ImGuiStyle& style = g.Style;
            ImVec2 pos = window->DC.CursorPos;
            ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

            const bool menuset_is_open = IsRootOfOpenMenuSet();
            if (menuset_is_open)
                ImGui::PushItemFlag(ImGuiItemFlags_NoWindowHoverableCheck, true);

            bool pressed;
            ImGui::PushID(label);
            if (!enabled)
                ImGui::BeginDisabled();

            const ImGuiSelectableFlags selectable_flags =
                ImGuiSelectableFlags_SelectOnRelease |
                ImGuiSelectableFlags_NoSetKeyOwner |
                ImGuiSelectableFlags_SetNavIdOnHover;

            float circle_size = g.FontSize * 0.55f; // размер кружка
            float spacing = style.ItemSpacing.x;    // отступ
            float total_w = circle_size + spacing + label_size.x;

            pressed = ImGui::Selectable("", false, selectable_flags | ImGuiSelectableFlags_SpanAvailWidth,
                ImVec2(total_w, label_size.y));

            if (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_Visible)
            {
                // Центр кружка
                ImVec2 center = pos + ImVec2(circle_size * 0.5f, label_size.y * 0.5f + g.FontSize * 0.05f);

                if (selected)
                    window->DrawList->AddCircleFilled(center, circle_size * 0.5f, ImGui::GetColorU32(ImGuiCol_Text), 16);
                else
                    window->DrawList->AddCircle(center, circle_size * 0.5f, ImGui::GetColorU32(ImGuiCol_Text), 16, 1.6f);

                // Текст справа от кружка
                ImGui::RenderText(pos + ImVec2(circle_size + spacing, 0.0f), label);

                // Шорткат справа (если есть)
                if (shortcut && shortcut[0])
                {
                    ImVec2 shortcut_size = ImGui::CalcTextSize(shortcut);
                    ImVec2 shortcut_pos = ImVec2(window->WorkRect.Max.x - shortcut_size.x - style.ItemSpacing.x,
                        pos.y);
                    ImGui::PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]);
                    ImGui::RenderText(shortcut_pos, shortcut);
                    ImGui::PopStyleColor();
                }
            }

            IMGUI_TEST_ENGINE_ITEM_INFO(g.LastItemData.ID, label,
                g.LastItemData.StatusFlags | ImGuiItemStatusFlags_Checkable | (selected ? ImGuiItemStatusFlags_Checked : 0));

            if (!enabled)
                ImGui::EndDisabled();
            ImGui::PopID();
            if (menuset_is_open)
                ImGui::PopItemFlag();

            return pressed;
        }

        bool IconButton(const char* label, unsigned long long* texture_id, const float& size_x, const float& size_y) {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            ImGuiID id = window->GetID(label);
            const ImGuiStyle& style = g.Style;

            // Вычисляем размер кнопки
            const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);
            ImVec2 actual_size = ImGui::CalcItemSize({size_x, size_y}, label_size.x + style.FramePadding.x * 2.0f + 20.0f,
                label_size.y + style.FramePadding.y * 2.0f);

            const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + actual_size);
            ImGui::ItemSize(bb, style.FramePadding.y);
            if (!ImGui::ItemAdd(bb, id))
                return false;

            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

            const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive :
                hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            ImGui::RenderNavHighlight(bb, ImGui::GetID(label));
            ImGui::RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);

            if (texture_id) {
                ImGui::RenderText(ImVec2(bb.Min.x + style.FramePadding.x + 20,
                    bb.Min.y + style.FramePadding.y), label);
                ImGui::GetWindowDrawList()->AddImage(texture_id,
                    ImVec2(bb.Min.x + style.FramePadding.x,
                        bb.Min.y + style.FramePadding.y),
                    ImVec2(bb.Min.x + style.FramePadding.x + 16,
                        bb.Min.y + style.FramePadding.y + 16));
            }
            else {
                ImGui::RenderText(ImVec2(bb.Min.x + style.FramePadding.x,
                    bb.Min.y + style.FramePadding.y), label);
            }

            return pressed;
        }

    }

	

}
