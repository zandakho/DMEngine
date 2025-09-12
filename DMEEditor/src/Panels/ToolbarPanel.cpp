#include "dmepch.h"
#include "ToolbarPanel.h"
#include <ImGui/imgui.h>
#include "DME/ImGui/ImGuiDMEEditor.h"
#include "ImGuizmo.h"

namespace DME {

	void ToolbarPanel::SetIcons(const Ref<Texture2D>& cursor, const Ref<Texture2D>& move, const Ref<Texture2D>& rotate, const Ref<Texture2D>& scale, const Ref<Texture2D>& play)
	{
		m_IconCursor = cursor; m_IconMove = move; m_IconRotate = rotate; m_IconScale = scale; m_IconPlay = play;
	}

	void ToolbarPanel::OnImGuiRender()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar;

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
		ImGui::Begin("EditorToolbar", nullptr, windowFlags);

		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(9, 5));
		if (m_GizmoTypePtr)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, (*m_GizmoTypePtr == -1) ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			if (ImGuiDMEEditor::IconButton("##Cursor", reinterpret_cast<uint64_t*>((uintptr_t)(m_IconCursor ? m_IconCursor->GetRendererID() : 0)), { 25,25 })) {
				*m_GizmoTypePtr = -1;
			}
			ImGui::PopStyleColor(); ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, (*m_GizmoTypePtr == ImGuizmo::OPERATION::TRANSLATE) ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			if (ImGuiDMEEditor::IconButton("##Move", reinterpret_cast<uint64_t*>((uintptr_t)(m_IconMove ? m_IconMove->GetRendererID() : 0)), { 25,25 })) {
				*m_GizmoTypePtr = ImGuizmo::OPERATION::TRANSLATE;
			}
			ImGui::PopStyleColor(); ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, (*m_GizmoTypePtr == ImGuizmo::OPERATION::ROTATE) ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			if (ImGuiDMEEditor::IconButton("##Rotate", reinterpret_cast<uint64_t*>((uintptr_t)(m_IconRotate ? m_IconRotate->GetRendererID() : 0)), { 25,25 })) {
				*m_GizmoTypePtr = ImGuizmo::OPERATION::ROTATE;
			}
			ImGui::PopStyleColor(); ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_Button, (*m_GizmoTypePtr == ImGuizmo::OPERATION::SCALE) ? ImVec4(0.2f, 0.6f, 0.9f, 0.5f) : ImVec4(0.2f, 0.2f, 0.2f, 0.5f));
			if (ImGuiDMEEditor::IconButton("##Scale", reinterpret_cast<uint64_t*>((uintptr_t)(m_IconScale ? m_IconScale->GetRendererID() : 0)), { 25,25 })) {
				*m_GizmoTypePtr = ImGuizmo::OPERATION::SCALE;
			}
			ImGui::PopStyleColor();
		}

		ImGui::PopStyleVar();
		ImGui::End();
	}

}
