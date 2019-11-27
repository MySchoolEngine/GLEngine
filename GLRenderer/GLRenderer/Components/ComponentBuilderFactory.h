#pragma once

#include <Entity/IComponent.h>

namespace GLEngine::GLRenderer::Components {

class C_ComponentBuilderFactory : public Entity::I_ComponentBuilderFactory
{
public:
	virtual std::unique_ptr<Entity::I_ComponenetBuilder> GetFactory(const std::string& name) override;
};

}