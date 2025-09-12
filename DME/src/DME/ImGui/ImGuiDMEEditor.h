#pragma once

#include <glm/glm.hpp>

namespace DME
{
	namespace ImGuiDMEEditor
	{
		bool Checkbox(const char* label, bool* v);
		bool CheckboxAsRadio(const char* label, bool* value, unsigned long long* iconTexture = nullptr);
		bool RadioButton(const char* label, bool active);
		bool MenuItemEx(const char* label, const char* icon, const char* shortcut = NULL, bool selected = false, bool enabled = true);
		bool IconButton(const char* label, unsigned long long* texture_id = nullptr, glm::vec2 size = { 0.0f, 0.0f }, glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
		bool IconButtonWithText(const char* label, unsigned long long* texture_id = nullptr);
		bool CardWithAssetType(const char* name_id, void* image, const char* type, const char* hint, const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec4& tint_col, int flags);
		bool FolderCard(const char* name_id, void* image, const char* type, const char* hint, const glm::vec2& uv0, const glm::vec2& uv1, int flags);
	}

}


