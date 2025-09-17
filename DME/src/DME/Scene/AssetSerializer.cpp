#include "dmepch.h"
#include "DME/Scene/AssetSerializer.h"

#include "DME/Scene/Entity.h"
#include "DME/Scene/Components.h"
#include "DME/Renderer/Texture.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML
{
	template<>
	struct convert<glm::vec2>
	{
		static Node encode(const glm::vec2& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec2& rhs)
		{
			if (!node.IsSequence() || node.size() != 2)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}

namespace DME
{
	void AssetSerializer::Serialize(Entity entity, const std::string& filepath)
	{
		DME_CORE_ASSERT(entity.HasComponent<IDComponent>())

		YAML::Emitter out;
		out << YAML::BeginMap;

		// Keep original UUID in file (but on instantiate we create a new entity/UUID)
		out << YAML::Key << "Entity" << YAML::Value << entity.GetUUID();

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Tag" << YAML::Value << entity.GetComponent<TagComponent>().Tag;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<TransformComponent>())
		{
			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;
			out << YAML::Key << "Position" << YAML::Value << tc.Position;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;

			auto& cameraComponent = entity.GetComponent<CameraComponent>();
			auto& camera = cameraComponent.Camera;

			out << YAML::Key << "Camera" << YAML::Value;
			out << YAML::BeginMap;

			out << YAML::Key << "ProjectionType" << YAML::Value << (int)camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();

			out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();

			out << YAML::Key << "Primary" << YAML::Value << cameraComponent.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cameraComponent.FixedAspectRatio;

			out << YAML::EndMap;
			out << YAML::EndMap;
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;

			auto& src = entity.GetComponent<SpriteRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << src.Color;
			if (src.Texture)
				out << YAML::Key << "TexturePath" << YAML::Value << src.Texture->GetPath();
			out << YAML::Key << "TilingFactor" << YAML::Value << src.TilingFactor;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleRendererComponent>())
		{
			out << YAML::Key << "CircleRendererComponent";
			out << YAML::BeginMap;

			auto& crc = entity.GetComponent<CircleRendererComponent>();
			out << YAML::Key << "Color" << YAML::Value << crc.Color;
			out << YAML::Key << "Thickness" << YAML::Value << crc.Thickness;
			out << YAML::Key << "Fade" << YAML::Value << crc.Fade;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<Rigidbody2DComponent>())
		{
			out << YAML::Key << "Rigidbody2DComponent";
			out << YAML::BeginMap;

			auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
			std::string body = (rb2d.Type == Rigidbody2DComponent::BodyType::Static ? "Static" : (rb2d.Type == Rigidbody2DComponent::BodyType::Dynamic ? "Dynamic" : "Kinematic"));
			out << YAML::Key << "BodyType" << YAML::Value << body;
			out << YAML::Key << "FixedRotation" << YAML::Value << rb2d.FixedRotation;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<BoxCollider2DComponent>())
		{
			out << YAML::Key << "BoxCollider2DComponent";
			out << YAML::BeginMap;

			auto& bc = entity.GetComponent<BoxCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << bc.Offset;
			out << YAML::Key << "Size" << YAML::Value << bc.Size;
			out << YAML::Key << "Density" << YAML::Value << bc.Density;
			out << YAML::Key << "Friction" << YAML::Value << bc.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << bc.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << bc.RestitutionThreshold;

			out << YAML::EndMap;
		}

		if (entity.HasComponent<CircleCollider2DComponent>())
		{
			out << YAML::Key << "CircleCollider2DComponent";
			out << YAML::BeginMap;

			auto& cc = entity.GetComponent<CircleCollider2DComponent>();
			out << YAML::Key << "Offset" << YAML::Value << cc.Offset;
			out << YAML::Key << "Radius" << YAML::Value << cc.Radius;
			out << YAML::Key << "Density" << YAML::Value << cc.Density;
			out << YAML::Key << "Friction" << YAML::Value << cc.Friction;
			out << YAML::Key << "Restitution" << YAML::Value << cc.Restitution;
			out << YAML::Key << "RestitutionThreshold" << YAML::Value << cc.RestitutionThreshold;

			out << YAML::EndMap;
		}

		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();
	}

	bool AssetSerializer::Instantiate(const std::string& filepath, const Ref<Scene>& scene, const glm::vec3& position)
	{
		YAML::Node data;
		try
		{
			data = YAML::LoadFile(filepath);
		}
		catch (YAML::ParserException e)
		{
			DME_CORE_ERROR("Failed to parse dasset: {0}", e.what())
			return false;
		}

		YAML::Node entity = data;
		if (!entity)
		{
			DME_CORE_ERROR("DME asset file empty or invalid: {0}", filepath)
			return false;
		}

		std::string name;
		if (auto tagComponent = entity["TagComponent"])
		{
			if (tagComponent)
				name = tagComponent["Tag"].as<std::string>();
		}

		// Create a new entity (new UUID) in the provided scene
		Entity deserializedEntity = scene->CreateEntity(name);

		if (auto transformComponent = entity["TransformComponent"])
		{
			if (transformComponent)
			{
				auto& tc = deserializedEntity.GetComponent<TransformComponent>();
				tc.Position = transformComponent["Position"].as<glm::vec3>();
				tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
				tc.Scale = transformComponent["Scale"].as<glm::vec3>();

				// override position with supplied drop-position
				tc.Position = position;
			}
		}
		

		if (auto cameraComponent = entity["CameraComponent"])
		{
			if (cameraComponent)
			{
				auto& cc = deserializedEntity.AddComponent<CameraComponent>();

				auto cameraProps = cameraComponent["Camera"];
				cc.Camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(cameraProps["ProjectionType"].as<int>()));

				cc.Camera.SetPerspectiveVerticalFOV(cameraProps["PerspectiveFOV"].as<float>());
				cc.Camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
				cc.Camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

				cc.Camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
				cc.Camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
				cc.Camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

				cc.Primary = cameraComponent["Primary"].as<bool>();
				cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
			}
		}
		

		if (auto spriteRendererComponent = entity["SpriteRendererComponent"])
		{
			if (spriteRendererComponent)
			{
				auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
				src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				if (spriteRendererComponent["TexturePath"])
					src.Texture = Texture2D::Create(spriteRendererComponent["TexturePath"].as<std::string>());

				src.TilingFactor = spriteRendererComponent["TilingFactor"].as<float>();
			}
		}
		

		if (auto circleRendererComponent = entity["CircleRendererComponent"])
		{
			if (circleRendererComponent)
			{
				auto& crc = deserializedEntity.AddComponent<CircleRendererComponent>();
				crc.Color = circleRendererComponent["Color"].as<glm::vec4>();
				crc.Thickness = circleRendererComponent["Thickness"].as<float>();
				crc.Fade = circleRendererComponent["Fade"].as<float>();
			}
		}
		

		if (auto rigidbody2DComponent = entity["Rigidbody2DComponent"])
		{
			if (rigidbody2DComponent)
			{
				auto& rb2d = deserializedEntity.AddComponent<Rigidbody2DComponent>();
				std::string bodyType = rigidbody2DComponent["BodyType"].as<std::string>();
				if (bodyType == "Static") rb2d.Type = Rigidbody2DComponent::BodyType::Static;
				else if (bodyType == "Dynamic") rb2d.Type = Rigidbody2DComponent::BodyType::Dynamic;
				else rb2d.Type = Rigidbody2DComponent::BodyType::Kinematic;

				rb2d.FixedRotation = rigidbody2DComponent["FixedRotation"].as<bool>();
			}
		}
		

		if (auto boxColliderComponent = entity["BoxCollider2DComponent"])
		{
			if (boxColliderComponent)
			{
				auto& bc = deserializedEntity.AddComponent<BoxCollider2DComponent>();
				bc.Offset = boxColliderComponent["Offset"].as<glm::vec2>();
				bc.Size = boxColliderComponent["Size"].as<glm::vec2>();
				bc.Density = boxColliderComponent["Density"].as<float>();
				bc.Friction = boxColliderComponent["Friction"].as<float>();
				bc.Restitution = boxColliderComponent["Restitution"].as<float>();
				bc.RestitutionThreshold = boxColliderComponent["RestitutionThreshold"].as<float>();
			}
		}
		

		if (auto circleCollider2DComponent = entity["CircleCollider2DComponent"])
		{
			if (circleCollider2DComponent)
			{
				auto& cc2d = deserializedEntity.AddComponent<CircleCollider2DComponent>();
				cc2d.Offset = circleCollider2DComponent["Offset"].as<glm::vec2>();
				cc2d.Radius = circleCollider2DComponent["Radius"].as<float>();
				cc2d.Density = circleCollider2DComponent["Density"].as<float>();
				cc2d.Friction = circleCollider2DComponent["Friction"].as<float>();
				cc2d.Restitution = circleCollider2DComponent["Restitution"].as<float>();
				cc2d.RestitutionThreshold = circleCollider2DComponent["RestitutionThreshold"].as<float>();
			}
		}

		return true;
	}
}
