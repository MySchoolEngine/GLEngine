#include <EntityStdafx.h>

#include <Entity/ComponentManager.h>

namespace GLEngine::Entity {
//=================================================================================
C_ComponentManager::C_ComponentManager() = default;

//=================================================================================
C_ComponentManager::~C_ComponentManager()
{
	for (const auto& it : m_Components)
	{
		GLE_ASSERT(it.use_count() == 1, "Component still in use during engine destruciton!");
	}
	// clear it anyway
	m_Components.clear();
}

//=================================================================================
C_ComponentManager& C_ComponentManager::Instance()
{
	static C_ComponentManager instance; // Guaranteed to be destroyed.
										// Instantiated on first use.
	return instance;
}

//=================================================================================
void C_ComponentManager::RegisterComponent(const T_ComponentPtr& component)
{
	// @todo: unit test this
	GLE_ASSERT(std::find(m_Components.begin(), m_Components.end(), component) == m_Components.end(), "Component already registered");
	m_Components.push_back(component);
}

//=================================================================================
void C_ComponentManager::UnregisterComonent(const T_ComponentPtr& component)
{
	// @todo: unit test this
	GLE_ASSERT(std::find(m_Components.begin(), m_Components.end(), component) != m_Components.end(), "Component already registered");
	m_Components.erase(std::remove(m_Components.begin(), m_Components.end(), component));
}
} // namespace GLEngine::Entity