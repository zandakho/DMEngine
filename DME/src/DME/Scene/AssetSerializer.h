#pragma once

#include "DME/Scene/Scene.h"

namespace DME
{
	class AssetSerializer
	{
	public:
		static void Serialize(Entity entity, const std::string& filepath);

		static bool Instantiate(const std::string& filepath, const Ref<Scene>& scene, const glm::vec3& position = { 0.0f, 0.0f, 0.0f });
	};
}
