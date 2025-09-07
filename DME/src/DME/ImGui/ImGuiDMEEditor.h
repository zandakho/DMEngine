#pragma once



namespace DME
{
	namespace ImGuiDMEEditor
	{
		bool Checkbox(const char* label, bool* v);
		bool CheckboxAsRadio(const char* label, bool* value, unsigned long long* iconTexture = nullptr);
		bool RadioButton(const char* label, bool active);
		bool MenuItemEx(const char* label, const char* icon, const char* shortcut = NULL, bool selected = false, bool enabled = true);
		bool IconButton(const char* label, unsigned long long* texture_id = nullptr, const float& size_x = { 0.0f }, const float& size_y = { 0.0f });
	}
}


