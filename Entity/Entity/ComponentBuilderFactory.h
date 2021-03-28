#pragma once

namespace GLEngine::Entity {
class I_ComponenetBuilder;
class I_Entity;

//=================================================================================
class I_ComponentBuilderFactory {
public:
	[[nodiscard]] virtual std::unique_ptr<I_ComponenetBuilder> GetFactory(const std::string& name)													  = 0;
	virtual void											   ConstructFromFile(std::shared_ptr<I_Entity> entity, const std::filesystem::path& file) = 0;
};

} // namespace GLEngine::Entity
