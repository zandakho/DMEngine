#pragma once

#include "DME/Scene/Entity.h"

namespace DME
{

	class ScriptableEntity
	{
	public:

		virtual ~ScriptableEntity() {}

		template<typename T>
		T& GetComponent()
		{
			return m_Entity.GetComponent<T>();
		}

	protected:

		virtual void OnCreate() {}
		virtual void OnDestroy() {}
		virtual void OnUpdate(TimeStep ts) {}


	private:
		Entity m_Entity;
		friend class Scene;
	};

}