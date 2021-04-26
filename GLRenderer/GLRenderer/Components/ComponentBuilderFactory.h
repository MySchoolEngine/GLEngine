#pragma once

#include <Entity/ComponentBuilderFactory.h>

namespace GLEngine::GLRenderer::Components {

class C_ComponentBuilderFactory : public Entity::I_ComponentBuilderFactory {
public:
	[[nodiscard]] virtual std::unique_ptr<Entity::I_ComponenetBuilder> GetFactory(const std::string& name) override;
	void															   ConstructFromFile(std::shared_ptr<Entity::I_Entity> entity, const std::filesystem::path& file) override;
};

} // namespace GLEngine::GLRenderer::Components
