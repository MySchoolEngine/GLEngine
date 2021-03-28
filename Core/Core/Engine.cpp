#include <CoreStdAfx.h>

#include <Core/Engine.h>

#include <Entity/ComponentBuilder.h>


namespace GLEngine::Core {

//=================================================================================
C_Engine::C_Engine()
{
}

//=================================================================================
C_Engine& C_Engine::Instance()
{
	static C_Engine instance; // Guaranteed to be destroyed.
							  // Instantiated on first use.
	return instance;
}

//=================================================================================
void C_Engine::RegisterGlobalComponentBuilder(const std::string& name, Entity::I_ComponenetBuilder* builder)
{
	m_GlobalComponentBuilders.insert({name, builder});
}

//=================================================================================
Entity::I_ComponenetBuilder* C_Engine::GetGlobalComponentBuilder(const std::string& name) const
{
	auto it = m_GlobalComponentBuilders.find(name);
	if (it != m_GlobalComponentBuilders.end())
		return it->second;

	return nullptr;
}

} // namespace GLEngine::Core
