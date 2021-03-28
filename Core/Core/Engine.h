#pragma once

#include <Core/CoreApi.h>

namespace GLEngine::Entity {
class I_ComponenetBuilder;
}

namespace GLEngine::Core {

// serve as main share-point between modules, may be subject of huge refactoring
class CORE_API_EXPORT C_Engine {
public:
	// Singleton stuff
	C_Engine(C_Engine const&)			   = delete;
	void						   operator=(C_Engine const&) = delete;
	[[nodiscard]] static C_Engine& Instance();

	void						 RegisterGlobalComponentBuilder(const std::string& name, Entity::I_ComponenetBuilder* builder);
	Entity::I_ComponenetBuilder* GetGlobalComponentBuilder(const std::string& name) const;

private:
	C_Engine();
	std::map<std::string, Entity::I_ComponenetBuilder*> m_GlobalComponentBuilders; // TODO!!! Memory leak!
};

} // namespace GLEngine::Core
