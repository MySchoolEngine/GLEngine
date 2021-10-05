#include <EntityStdafx.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <pugixml.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <rttr/registration>

RTTR_REGISTRATION
{
	rttr::registration::class_<GLEngine::Entity::I_ComponenetBuilder>("I_ComponenetBuilder")
		.method("Build", &GLEngine::Entity::I_ComponenetBuilder::Build);

	rttr::registration::class_<GLEngine::Entity::I_Component>("I_Component");
}

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
		ImVec2 padding{20, 0};
		ImVec2 p0 = ImGui::GetCursorScreenPos();
		ImGui::SetCursorScreenPos(p0 + padding);
		ImGui::BeginGroup();
		m_Transformation.Draw();
		DebugDrawGUI();
		ImGui::EndGroup();
	}
}

} // namespace GLEngine::Entity