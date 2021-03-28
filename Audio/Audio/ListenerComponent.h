#pragma once

#include <Audio/AudioApi.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>
#include <Entity/ComponentBuilder.h>

namespace GLEngine::Audio {

class C_ListenerComponent : public Entity::I_Component {
public:
	explicit C_ListenerComponent(std::shared_ptr<Entity::I_Entity> owner);

	[[nodiscard]] glm::vec3 C_ListenerComponent::GetPosition() const;

	virtual [[nodiscard]] Entity::E_ComponentType	  GetType() const override;
	virtual [[nodiscard]] bool						  HasDebugDrawGUI() const override;
	virtual [[nodiscard]] std::string_view			  GetDebugComponentName() const override;
	virtual [[nodiscard]] Physics::Primitives::S_AABB GetAABB() const override;
};


class C_ListenerBuilder : public Entity::I_ComponenetBuilder {
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};

} // namespace GLEngine::Audio

namespace GLEngine {
template <> class ComponenetBase<Entity::E_ComponentType::AudioListener> {
public:
	using type = Audio::C_ListenerComponent;
};
} // namespace GLEngine