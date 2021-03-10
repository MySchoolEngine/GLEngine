#include <EntityStdafx.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <imgui.h>

namespace GLEngine::Entity {

//=================================================================================
I_Component::I_Component(std::shared_ptr<I_Entity> owner)
	: m_Owner(owner)
	, m_Transformation(glm::mat4(1.f))
{
}

//=================================================================================
std::shared_ptr<I_Entity> I_Component::GetOwner() const
{
	// component without entity is invalid state
	if (auto owner = m_Owner.lock())
	{
		return owner;
	}
	GLE_ASSERT(m_Owner.expired(), "Component has no owner.");
	return nullptr;
}

//=================================================================================
const glm::mat4 I_Component::GetComponentModelMatrix() const
{
	const auto owner = GetOwner();
	if (owner)
	{
		return owner->GetModelMatrix() * m_Transformation.GetMatrix();
	}
	return m_Transformation.GetMatrix();
}

//=================================================================================
void I_Component::DebugDrawComponentGUI()
{
	if (::ImGui::CollapsingHeader(GetDebugComponentName().data()))
	{
		m_Transformation.Draw();
		DebugDrawGUI();
	}
}

//=================================================================================
Utils::C_BitField<GUI::Input::C_Transformations::E_Transorms> I_Component::GetAllowedTransforms() const
{
	using namespace GUI::Input;
	return {C_Transformations::E_Transorms::Translate, C_Transformations::E_Transorms::Rotate, C_Transformations::E_Transorms::Scale};
}

} // namespace GLEngine::Entity