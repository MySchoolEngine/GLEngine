#pragma once

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <Physics/Primitives/AABB.h>

namespace GLEngine::Entity {

class C_DebugGUIComponent : public I_Component {
public:
	C_DebugGUIComponent(std::shared_ptr<I_Entity> owner);
	virtual ~C_DebugGUIComponent(); // = default;
	virtual E_ComponentType GetType() const override;

	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;
	[[nodiscard]] virtual bool						  HasDebugDrawGUI() const override;
	void											  Toggle();

	void PostUpdate() override;

	virtual void						   DrawContents();
	[[nodiscard]] virtual std::string_view GetDebugComponentName() const override;

protected:
	std::string m_Title;
	bool		m_Show;
};
} // namespace GLEngine::Entity

namespace GLEngine {
template <> class ComponenetBase<Entity::E_ComponentType::DebugGUI> {
public:
	using type = Entity::C_DebugGUIComponent;
};
} // namespace GLEngine
