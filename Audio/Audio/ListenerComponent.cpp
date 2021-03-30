#include <AudioStdAfx.h>

#include <Audio/AudioSystemManager.h>
#include <Audio/ListenerComponent.h>

#include <Physics/Primitives/AABB.h>

#include <Core/CoreRegistrations.h>

#include <Utils/Parsing/MatrixParse.h>

namespace GLEngine::Audio {

REGISTER_GLOBAL_COMPONENT_BUILDER("AudioListener", C_ListenerBuilder);

//=================================================================================
C_ListenerComponent::C_ListenerComponent(std::shared_ptr<Entity::I_Entity> owner)
	: Entity::I_Component(std::move(owner))
	, m_Activate("Activate", [&]() { C_AudioSystemManager::Instance().ActivateListener(shared_from_this()); })
	, m_LastPosition(GetPosition())
{
}

//=================================================================================
Physics::Primitives::S_AABB C_ListenerComponent::GetAABB() const
{
	Physics::Primitives::S_AABB ret;
	ret.Add(GetPosition());
	return ret;
}

//=================================================================================
glm::vec3 C_ListenerComponent::GetPosition() const
{
	return glm::vec3(GetComponentModelMatrix()[3]);
}

//=================================================================================
std::string_view C_ListenerComponent::GetDebugComponentName() const
{
	return "Audio listener";
}

//=================================================================================
[[nodiscard]] Entity::E_ComponentType C_ListenerComponent::GetType() const
{
	return Entity::E_ComponentType::AudioListener;
}

//=================================================================================
[[nodiscard]] bool C_ListenerComponent::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
void C_ListenerComponent::DebugDrawGUI(GUI::C_GUIManager* guiMGR /*= nullptr*/)
{
	const auto isActive = C_AudioSystemManager::Instance().GetActiveListener() == shared_from_this();
	if (!isActive)
		m_Activate.Draw();
}

//=================================================================================
void C_ListenerComponent::Update()
{
	m_LastPosition = GetPosition(); // todo could be wrong place
}

//=================================================================================
glm::vec3 C_ListenerComponent::GetVelocity() const
{
	return (GetPosition() - m_LastPosition);
}

//=================================================================================
std::shared_ptr<Entity::I_Component> C_ListenerBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	auto listener = std::make_shared<Audio::C_ListenerComponent>(owner);
	listener->SetComponentMatrix(Utils::Parsing::C_MatrixParser::ParseTransformation(node));
	return listener;
}

} // namespace GLEngine::Audio