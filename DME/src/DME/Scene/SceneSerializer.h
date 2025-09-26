#pragma once

#include "DME/Scene/Scene.h"

namespace DME
{
	class SceneSerializer
	{
	public:
		SceneSerializer(const Ref<Scene>& scene);


		void Serialize(const std::string& filepath);
		void SerializeRuntime(const std::string& filepath);


		bool Deserialize(const std::string& filepath);
		bool DeSerializeRuntime(const std::string& filepath);
	private:

		Ref<Scene> m_Scene;
	};
}

