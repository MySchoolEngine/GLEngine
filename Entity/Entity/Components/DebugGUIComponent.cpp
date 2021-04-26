#include <EntityStdAfx.h>

#include <Entity/ComponentBuilder.h>
#include <Entity/Components/DebugGUIComponent.h>

#include <Core/CoreRegistrations.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
C_DebugGUIComponent::C_DebugGUIComponent(std::shared_ptr<I_Entity> owner)
	: I_Component(owner)
	, m_Title(owner->GetName())
	, m_Show(false)
{
}

//=================================================================================
C_DebugGUIComponent::~C_DebugGUIComponent() = default;

//=================================================================================
std::string_view C_DebugGUIComponent::GetDebugComponentName() const
{
	return "Debug entity component";
}

//=================================================================================
void C_DebugGUIComponent::DrawContents()
{
	if (auto entity = GetOwner())
	{
		for (auto& component : *entity)
		{
			if (component.second->HasDebugDrawGUI())
				component.second->DebugDrawComponentGUI();
		}
	}
}

//=================================================================================
void C_DebugGUIComponent::PostUpdate()
{
	if (m_Show)
	{
		ImGui::Begin(m_Title.c_str(), &m_Show);
		DrawContents();
		ImGui::End();
	}
}

//=================================================================================
void C_DebugGUIComponent::Toggle()
{
	m_Show = !m_Show;
}

//=================================================================================
bool C_DebugGUIComponent::HasDebugDrawGUI() const
{
	return false;
}

//=================================================================================
Physics::Primitives::S_AABB C_DebugGUIComponent::GetAABB() const
{
	return Physics::Primitives::S_AABB();
}

//=================================================================================
E_ComponentType C_DebugGUIComponent::GetType() const
{
	return E_ComponentType::DebugGUI;
}

//=================================================================================
//=================================================================================
//=================================================================================
class C_GUIDebugBuilder : public I_ComponenetBuilder {
public:
	virtual std::shared_ptr<I_Component> Build(const pugi::xml_node& node, std::shared_ptr<I_Entity> owner) override {
		return std::make_shared<C_DebugGUIComponent>(owner);
	}
};
REGISTER_GLOBAL_COMPONENT_BUILDER("debug", C_GUIDebugBuilder);
} // namespace GLEngine::Entity
