#pragma once

#include <Entity/EntityApi.h>
#include <Entity/IComponent.h>

namespace GLEngine::Entity {

/**
 * @todo
 * This should be singleton, but! with controlled destruction time.
 * Also, we should have entity manager, which will be destroyed before
 * destruction of this.
 */
class ENTITY_API_EXPORT C_ComponentManager {
public:
	// Singleton stuff
	C_ComponentManager(C_ComponentManager const&) = delete;
	void					   operator=(C_ComponentManager const&) = delete;
	static C_ComponentManager& Instance();

	void RegisterComponent(const T_ComponentPtr& component);
	void UnregisterComonent(const T_ComponentPtr& component);

private:
	C_ComponentManager();
	~C_ComponentManager();

	std::vector<T_ComponentPtr> m_Components;

	friend class I_Component;
};
} // namespace GLEngine::Entity