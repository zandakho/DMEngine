#include "dmepch.h"

#include "DME/Scene/Entity.h"

namespace DME
{
	Entity::Entity(entt::entity handle, Scene* scene) : m_EntityHandle(handle), m_Scene(scene) {};

}