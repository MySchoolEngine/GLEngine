#pragma once

namespace pugi {
class xml_node;
}

namespace GLEngine::Entity {

class I_Component;
class I_Entity;

//=================================================================================
class I_ComponenetBuilder {
public:
	virtual std::shared_ptr<I_Component> Build(const pugi::xml_node& node, std::shared_ptr<I_Entity> owner) = 0;
};
} // namespace GLEngine::Entity
