#include <EntityStdafx.h>

#include <Entity/ComponentRegistry.h>

#include <algorithm>

namespace GLEngine::Entity {

//=================================================================================
C_ComponentRegistry& C_ComponentRegistry::Instance()
{
	static C_ComponentRegistry instance;
	return instance;
}

//=================================================================================
const S_ComponentRegistryEntry* C_ComponentRegistry::FindByName(std::string_view name) const
{
	const auto it = std::find_if(m_Entries.begin(), m_Entries.end(), [name](const S_ComponentRegistryEntry& entry) { return entry.name == name; });
	return it == m_Entries.end() ? nullptr : &(*it);
}

} // namespace GLEngine::Entity
