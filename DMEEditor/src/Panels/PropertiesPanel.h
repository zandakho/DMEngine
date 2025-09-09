#pragma once

#include "DME/Core/Base.h"
#include "DME/Scene/Scene.h"
#include "DME/Scene/Entity.h"

namespace DME {

    class PropertiesPanel {
    public:
        PropertiesPanel() = default;

        void SetContext(Entity entity) { m_SelectedEntity = entity; }
        void OnImGuiRender();

    private:
        template<typename T, typename UFunction>
        void DrawComponent(const std::string& name, Entity entity, UFunction uifunction);

        template<typename T>
        void DisplayAddComponentEntry(const std::string& entryName);

        static void DrawVec3Control(const std::string& label, glm::vec3& values);

    private:
        Entity m_SelectedEntity;
    };

}
