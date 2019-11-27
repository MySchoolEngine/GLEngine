#pragma once

namespace GLEngine::Entity {
class I_Component;
}

namespace pugi {
class xml_node;
}

namespace GLEngine::GLRenderer::Components {

class I_ComponenetBuilder
{
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node) = 0;
};

class C_ComponentBuilderFactory
{
public:
	static I_ComponenetBuilder* GetFactory(const std::string& name);
};

}