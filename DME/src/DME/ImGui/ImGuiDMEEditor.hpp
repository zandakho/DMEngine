#pragma once

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"

namespace DME
{
    namespace ImGuiDMEEditor
    {

		inline bool BeginMenuBar(const char* str_id)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return false;
			if (!(window->Flags & ImGuiWindowFlags_MenuBar))
				return false;

			IM_ASSERT(!window->DC.MenuBarAppending);
			ImGui::BeginGroup(); // Backup position on layer 0 // FIXME: Misleading to use a group for that backup/restore
			ImGui::PushID(str_id);

			// We don't clip with current window clipping rectangle as it is already set to the area below. However we clip with window full rect.
			// We remove 1 worth of rounding to Max.x to that text in long menus and small windows don't tend to display over the lower-right rounded area, which looks particularly glitchy.
			const float border_top = ImMax(IM_ROUND(window->WindowBorderSize * 0.5f - window->TitleBarHeight), 0.0f);
			const float border_half = IM_ROUND(window->WindowBorderSize * 0.5f);
			ImRect bar_rect = window->MenuBarRect();
			ImRect clip_rect(ImFloor(bar_rect.Min.x + border_half), ImFloor(bar_rect.Min.y + border_top), ImFloor(ImMax(bar_rect.Min.x, bar_rect.Max.x - ImMax(window->WindowRounding, border_half))), ImFloor(bar_rect.Max.y));
			clip_rect.ClipWith(window->OuterRectClipped);
			ImGui::PushClipRect(clip_rect.Min, clip_rect.Max, false);

			// We overwrite CursorMaxPos because BeginGroup sets it to CursorPos (essentially the .EmitItem hack in EndMenuBar() would need something analogous here, maybe a BeginGroupEx() with flags).
			window->DC.CursorPos = window->DC.CursorMaxPos = ImVec2(bar_rect.Min.x + window->DC.MenuBarOffset.x, bar_rect.Min.y + window->DC.MenuBarOffset.y);
			window->DC.LayoutType = ImGuiLayoutType_Horizontal;
			window->DC.IsSameLine = false;
			window->DC.NavLayerCurrent = ImGuiNavLayer_Menu;
			window->DC.MenuBarAppending = true;
			ImGui::AlignTextToFramePadding();
			return true;
		}

		inline void EndMenuBar()
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return;
			ImGuiContext& g = *GImGui;

			IM_MSVC_WARNING_SUPPRESS(6011); // Static Analysis false positive "warning C6011: Dereferencing NULL pointer 'window'"
			IM_ASSERT(window->Flags & ImGuiWindowFlags_MenuBar);
			IM_ASSERT(window->DC.MenuBarAppending);

			// Nav: When a move request within one of our child menu failed, capture the request to navigate among our siblings.
			if (ImGui::NavMoveRequestButNoResultYet() && (g.NavMoveDir == ImGuiDir_Left || g.NavMoveDir == ImGuiDir_Right) && (g.NavWindow->Flags & ImGuiWindowFlags_ChildMenu))
			{
				// Try to find out if the request is for one of our child menu
				ImGuiWindow* nav_earliest_child = g.NavWindow;
				while (nav_earliest_child->ParentWindow && (nav_earliest_child->ParentWindow->Flags & ImGuiWindowFlags_ChildMenu))
					nav_earliest_child = nav_earliest_child->ParentWindow;
				if (nav_earliest_child->ParentWindow == window && nav_earliest_child->DC.ParentLayoutType == ImGuiLayoutType_Horizontal && (g.NavMoveFlags & ImGuiNavMoveFlags_Forwarded) == 0)
				{
					// To do so we claim focus back, restore NavId and then process the movement request for yet another frame.
					// This involve a one-frame delay which isn't very problematic in this situation. We could remove it by scoring in advance for multiple window (probably not worth bothering)
					const ImGuiNavLayer layer = ImGuiNavLayer_Menu;
					IM_ASSERT(window->DC.NavLayersActiveMaskNext & (1 << layer)); // Sanity check (FIXME: Seems unnecessary)
					ImGui::FocusWindow(window);
					ImGui::SetNavID(window->NavLastIds[layer], layer, 0, window->NavRectRel[layer]);
					// FIXME-NAV: How to deal with this when not using g.IO.ConfigNavCursorVisibleAuto?
					if (g.NavCursorVisible)
					{
						g.NavCursorVisible = false; // Hide nav cursor for the current frame so we don't see the intermediary selection. Will be set again
						g.NavCursorHideFrames = 2;
					}
					g.NavHighlightItemUnderNav = g.NavMousePosDirty = true;
					ImGui::NavMoveRequestForward(g.NavMoveDir, g.NavMoveClipDir, g.NavMoveFlags, g.NavMoveScrollFlags); // Repeat
				}
			}

			ImGui::PopClipRect();
			ImGui::PopID();
			IM_MSVC_WARNING_SUPPRESS(6011); // Static Analysis false positive "warning C6011: Dereferencing NULL pointer 'window'"
			window->DC.MenuBarOffset.x = window->DC.CursorPos.x - window->Pos.x; // Save horizontal position so next append can reuse it. This is kinda equivalent to a per-layer CursorPos.

			// FIXME: Extremely confusing, cleanup by (a) working on WorkRect stack system (b) not using a Group confusingly here.
			ImGuiGroupData& group_data = g.GroupStack.back();
			group_data.EmitItem = false;
			ImVec2 restore_cursor_max_pos = group_data.BackupCursorMaxPos;
			window->DC.IdealMaxPos.x = ImMax(window->DC.IdealMaxPos.x, window->DC.CursorMaxPos.x - window->Scroll.x); // Convert ideal extents for scrolling layer equivalent.
			ImGui::EndGroup(); // Restore position on layer 0 // FIXME: Misleading to use a group for that backup/restore
			window->DC.LayoutType = ImGuiLayoutType_Vertical;
			window->DC.IsSameLine = false;
			window->DC.NavLayerCurrent = ImGuiNavLayer_Main;
			window->DC.MenuBarAppending = false;
			window->DC.CursorMaxPos = restore_cursor_max_pos;
		}

        inline bool Checkbox(const char* label, bool* v)
        {
			
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            const ImGuiID id = window->GetID(label);
            const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);

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
                ImGui::MultiSelectItemHeader(id, &checked, nullptr);

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

		inline bool CheckboxAsRadio(const char* label, bool* value, ImTextureID* iconTexture = nullptr)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            const ImGuiID id = window->GetID(label);

            const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
            const float circle_radius = 8.0f;
            const ImVec2 circle_size = ImVec2(circle_radius * 2, circle_radius * 2);
            const ImVec2 icon_size = iconTexture ? ImVec2(16.0f, 16.0f) : ImVec2(0.0f, 0.0f);

            const float circle_icon_spacing = iconTexture ? 8.0f : 0.0f;
            const float icon_text_spacing = 8.0f;

            const float total_width = circle_size.x + circle_icon_spacing + icon_size.x +
                icon_text_spacing + label_size.x;
            const float total_height = ImMax(ImMax(circle_size.y, icon_size.y), label_size.y);

            const ImVec2 total_size = ImVec2(total_width, total_height);

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

            const ImU32 circle_color = *value ?
                ImGui::GetColorU32(ImVec4(0.0f, 0.47f, 1.0f, 1.0f)) : 
                ImGui::GetColorU32(ImVec4(0.3f, 0.3f, 0.3f, 1.0f)); 

            const ImU32 circle_border = ImGui::GetColorU32(ImVec4(0.6f, 0.6f, 0.6f, 1.0f));

            if (hovered)
            {
                window->DrawList->AddRectFilled(
                    total_bb.Min,
                    total_bb.Max,
                    ImGui::GetColorU32(ImVec4(0.1f, 0.3f, 0.6f, 0.3f)),
                    4.0f
                );
            }

            window->DrawList->AddCircleFilled(circle_center, circle_radius, circle_color);
            window->DrawList->AddCircle(circle_center, circle_radius, circle_border, 0, 1.5f);

            if (iconTexture)
            {
                const ImVec2 icon_pos = ImVec2(
                    total_bb.Min.x + circle_size.x + circle_icon_spacing,
                    total_bb.Min.y + (total_height - icon_size.y) * 0.5f
                );

                ImU32 icon_color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
                if (!*value)
                {
                    icon_color = ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 0.8f));
                }

                window->DrawList->AddImage(iconTexture, icon_pos, icon_pos + icon_size,
                    ImVec2(0, 0), ImVec2(1, 1), icon_color);
            }

            if (label_size.x > 0.0f)
            {
                const float text_offset_x = circle_size.x + circle_icon_spacing + icon_size.x + icon_text_spacing;
                const ImVec2 text_pos = ImVec2(
                    total_bb.Min.x + text_offset_x,
                    total_bb.Min.y + (total_height - label_size.y) * 0.5f
                );

                const ImU32 text_color = *value ?
                    ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)) : 
                    ImGui::GetColorU32(ImVec4(0.8f, 0.8f, 0.8f, 1.0f));  

                window->DrawList->AddText(text_pos, text_color, label);
            }

            return pressed;
        }

		inline bool RadioButton(const char* label, bool active)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            const ImGuiStyle& style = g.Style;
            const ImGuiID id = window->GetID(label);
            const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);

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
        
		inline static bool IsRootOfOpenMenuSet()
        {
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = g.CurrentWindow;
            if ((g.OpenPopupStack.Size <= g.BeginPopupStack.Size) || (window->Flags & ImGuiWindowFlags_ChildMenu))
                return false;
           
            const ImGuiPopupData* upper_popup = &g.OpenPopupStack[g.BeginPopupStack.Size];
            if (window->DC.NavLayerCurrent != upper_popup->ParentNavLayer)
                return false;
            return upper_popup->Window && (upper_popup->Window->Flags & ImGuiWindowFlags_ChildMenu) && ImGui::IsWindowChildOf(upper_popup->Window, window, true, false);
        }

		inline bool MenuItemEx(const char* label, const char* icon, const char* shortcut = nullptr, bool selected = false, bool enabled = true)
        {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            ImGuiStyle& style = g.Style;
            ImVec2 pos = window->DC.CursorPos;
            ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);

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

            float circle_size = g.FontSize * 0.60f;
            float spacing = style.ItemSpacing.x;   
            float total_w = circle_size + spacing + label_size.x;

            pressed = ImGui::Selectable("", false, selectable_flags | ImGuiSelectableFlags_SpanAvailWidth,
                ImVec2(total_w, label_size.y));

            if (g.LastItemData.StatusFlags & ImGuiItemStatusFlags_Visible)
            {
                ImVec2 center = pos + ImVec2(circle_size * 0.5f, label_size.y * 0.5f + g.FontSize * 0.05f);

                if (selected)
					window->DrawList->AddCircleFilled(center, circle_size * 0.4f, ImColor(185, 185, 185, 255), 16);
                else
					window->DrawList->AddCircleFilled(center, circle_size * 0.4f, ImColor(185, 185, 185, 50), 16);

                ImGui::RenderText(pos + ImVec2(circle_size + spacing, 0.0f), label);

                if (shortcut && shortcut[0])
                {
                    ImVec2 shortcut_size = ImGui::CalcTextSize(shortcut);
                    ImVec2 shortcut_pos = ImVec2(window->WorkRect.Max.x - shortcut_size.x - spacing,
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

		inline bool IconButton(const char* label, ImTextureID* texture_id = nullptr, glm::vec2 size = { 0.0f, 0.0f } , glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f }) {
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiContext& g = *GImGui;
            ImGuiID id = window->GetID(label);
            const ImGuiStyle& style = g.Style;

            const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
            ImVec2 actual_size = ImGui::CalcItemSize(ImVec2(size.x, size.y), label_size.x + 10, label_size.y + 10);

            const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + actual_size);
            ImGui::ItemSize(bb, style.FramePadding.y);
            if (!ImGui::ItemAdd(bb, id))
                return false;

            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

            const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive :
                hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            ImGui::RenderNavHighlight(bb, ImGui::GetID(label));
            ImGui::RenderFrame(bb.Min, bb.Max, col, true, 3.0f);

            if (texture_id) 
            {
                window->DrawList->AddImage(texture_id, ImVec2(bb.Min.x + 5, bb.Min.y + 5), ImVec2(bb.Max.x - 5, bb.Max.y - 5), { 0, 1 }, { 1, 0 }, ImColor(color.r, color.g, color.b, color.a));
            }

            return pressed;
        }

		inline bool IconButtonWithText(const char* label, ImTextureID* texture_id = nullptr)
		{
			ImGuiWindow* window = ImGui::GetCurrentWindow();
			if (window->SkipItems)
				return false;

			ImGuiContext& g = *GImGui;
			ImGuiID id = window->GetID(label);
			const ImGuiStyle& style = g.Style;

			const ImVec2 label_size = ImGui::CalcTextSize(label, nullptr, true);
			ImVec2 actual_size = ImGui::CalcItemSize(ImVec2(40 + label_size.x, 28), 32, 28);

			const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + actual_size);
			ImGui::ItemSize(bb, style.FramePadding.y);
			if (!ImGui::ItemAdd(bb, id))
				return false;

			bool hovered, held;
			bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

			const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive :
				hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
			ImGui::RenderNavHighlight(bb, id);
			ImGui::RenderFrame(bb.Min, bb.Max, col, true, 3.0f);

			if (texture_id)
			{
				window->DrawList->AddImage(texture_id, ImVec2(bb.Min.x + 6, bb.Min.y + 6), ImVec2(bb.Min.x + 24, bb.Max.y - 6), {0, 1}, {1, 0});
                ImRect text_pos(ImVec2(bb.Min.x + 30, bb.Min.y + 5), ImVec2(bb.Max.x - 5, bb.Max.y - 5));
                ImGui::RenderTextClipped(text_pos.Min, text_pos.Max, label, nullptr, nullptr, ImVec2(0.0f, 0.0f), &text_pos);
			}

			return pressed;
		}

		inline bool CardWithAssetType(const char* name_id, void* image, const char* type, const char* extension, const char* hint, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec4& tint_col, ImGuiButtonFlags flags, const glm::vec4& asset_type_color = { 1.0f, 1.0f, 1.0f, 1.0f })
        {
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            std::string global_id = std::format("{0}##{1}", name_id, extension);
            ImGuiID id = ImGui::GetID(global_id.c_str());

            const ImVec2 name_id_size = ImGui::CalcTextSize(name_id, nullptr, true);

            const ImVec2 padding = g.Style.FramePadding;
            const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(120, 200));
            ImGui::ItemSize(bb);
            if (!ImGui::ItemAdd(bb, id))
                return false;

            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

            // Render
            const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            ImGui::RenderNavCursor(bb, id);

            window->DrawList->AddRectFilled(bb.Min + ImVec2(2, 2), bb.Max - ImVec2(2, 2), ImColor(0.15f, 0.15f, 0.15f, 1.0f), 3.0f, ImDrawFlags_RoundCornersAll);
            
            window->DrawList->AddRectFilled(bb.Min + ImVec2(5, 5), bb.Min + ImVec2(115, 115), ImColor(0.10f, 0.10f, 0.10f, 1.0f), 4.0f, ImDrawFlags_RoundCornersAll);
            window->DrawList->AddImageRounded(image, bb.Min + ImVec2(10, 10), bb.Min + ImVec2(110, 110), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y), ImColor(tint_col.r, tint_col.g, tint_col.b, tint_col.a), 4.0f, ImDrawFlags_RoundCornersAll);

            window->DrawList->AddRectFilled(bb.Min + ImVec2(5, 114), bb.Min + ImVec2(115, 117), ImColor(asset_type_color.r, asset_type_color.g, asset_type_color.b, asset_type_color.a), 3, ImDrawFlags_RoundCornersBottomLeft | ImDrawFlags_RoundCornersBottomRight);

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
			{
				ImRect text_bb(bb.Min + ImVec2(10, 120), bb.Min + ImVec2(110, 140));
				ImGui::RenderTextClipped(text_bb.Min, text_bb.Max, name_id, nullptr, nullptr, ImVec2(0.0f, 0.0f), &text_bb);
			}
			ImGui::PopFont();

			std::string cast_string = std::format("{0} ({1})", type, hint);
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]);
			{
				ImRect th_bb(bb.Min + ImVec2(5, 180), bb.Min + ImVec2(115, 200));
				ImGui::RenderTextClipped(th_bb.Min, th_bb.Max, cast_string.c_str(), nullptr, nullptr, ImVec2(0.5f, 0.0f), &th_bb);
			}
			ImGui::PopFont();

			window->DrawList->AddRect(bb.Min, bb.Max, ImColor(15, 15, 15, 255), 4.0f, ImDrawFlags_RoundCornersAll, 3.0f);


            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
            {
				ImGui::PushStyleColor(ImGuiCol_PopupBg, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
				ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 3.0f);
				if (ImGui::BeginTooltipEx(ImGuiTooltipFlags_OverridePrevious, ImGuiWindowFlags_None))
				{

					ImGui::Text("Name: %s", name_id);
					ImGui::Text("Extension: %s", extension);
					ImGui::Text("Type: %s", type);
					ImGui::Text("Directory: %s", hint);

					ImGui::EndTooltip();
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
            }
            return pressed;
        }

		inline bool FolderCard(const char* name_id, void* image, const char* type, const char* hint, const glm::vec2& uv0, const glm::vec2& uv1, ImGuiButtonFlags flags)
        {
            ImGuiContext& g = *GImGui;
            ImGuiWindow* window = ImGui::GetCurrentWindow();
            if (window->SkipItems)
                return false;

            ImGuiID id = ImGui::GetID(name_id);
            const ImVec2 name_id_size = ImGui::CalcTextSize(name_id, nullptr, true);

            const ImVec2 padding = g.Style.FramePadding;
            const ImRect bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(120, 200));
            ImGui::ItemSize(bb);
            if (!ImGui::ItemAdd(bb, id))
                return false;

            bool hovered, held;
            bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, flags);

            // Render
            const ImU32 col = ImGui::GetColorU32((held && hovered) ? ImGuiCol_ButtonActive : hovered ? ImGuiCol_ButtonHovered : ImGuiCol_Button);
            ImGui::RenderNavCursor(bb, id);

			if (hovered)
			{
				window->DrawList->AddRect(bb.Min, bb.Max, ImColor(15, 15, 15, 255), 4.0f, ImDrawFlags_RoundCornersAll, 3.0f);
				window->DrawList->AddRectFilled(bb.Min + ImVec2(2, 2), bb.Max - ImVec2(2, 2), ImColor(0.15f, 0.15f, 0.15f, 1.0f), 3.0f, ImDrawFlags_RoundCornersAll);
				window->DrawList->AddRectFilled(bb.Min, bb.Max, ((held && hovered) ? ImColor(20, 20, 20, 100) : hovered ? ImColor(20, 20, 20, 70) : ImColor(0, 0, 0, 0)));

				window->DrawList->AddRectFilled(bb.Min + ImVec2(5, 5), bb.Min + ImVec2(115, 115), ImColor(0.10f, 0.10f, 0.10f, 1.0f), 4.0f, ImDrawFlags_RoundCornersAll);
			}
            
            window->DrawList->AddImageRounded(image, bb.Min + ImVec2(10, 10), bb.Min + ImVec2(110, 110), ImVec2(uv0.x, uv0.y), ImVec2(uv1.x, uv1.y), ImColor(1.0f, 1.0f, 1.0f, 1.0f), 3.0f, ImDrawFlags_RoundCornersAll);

            ImRect text_bb(bb.Min + ImVec2(10, 120), bb.Min + ImVec2(110, 140));

			ImGui::RenderTextClipped(text_bb.Min,
				text_bb.Max,
				name_id,
				nullptr,
                nullptr,
				ImVec2(0.5f, 0.0f),
				&text_bb);

            return pressed;
        }

    }
}
