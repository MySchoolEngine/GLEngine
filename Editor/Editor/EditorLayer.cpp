#include <EditorStdafx.h>

#include <Editor/AnimationEditor.h>
#include <Editor/EditorLayer.h>
#include <Editor/Editors/WorldView/AABBEditor.h>
#include <Editor/Editors/WorldView/CurveEditor.h>
#include <Editor/Utils/MousePicking.h>

#include <Renderer/DebugDraw.h>

#include <Entity/BasicEntity.h>
#include <Entity/EntityManager.h>

#include <Core/EventSystem/EventDispatcher.h>

namespace GLEngine::Editor {

//=================================================================================
C_EditorLayer::C_EditorLayer(Renderer::I_DebugDraw&							 dd,
							 const Core::I_Input&							 directInput,
							 const Renderer::C_Viewport&					 viewport,
							 const std::shared_ptr<Entity::C_EntityManager>& world)
	: Core::C_Layer("Editor Layer")
	, m_dd(dd)
	, m_Input(directInput)
	, m_Viewport(viewport)
	, m_World(world)
	, m_CurveEditing(nullptr)
	, m_AnimationEditing(nullptr)
{
}

//=================================================================================
C_EditorLayer::~C_EditorLayer() = default;

//=================================================================================
void C_EditorLayer::OnEvent(Core::I_Event& event)
{
	if (m_CurveEditing)
		m_CurveEditing->OnEvent(event);
	if (m_AABBEditing)
		m_AABBEditing->OnEvent(event);

	if (m_EntityGizmo)
	{
		m_EntityGizmo->OnEvent(event);
	}

	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_EntityEvent>(std::bind(&C_EditorLayer::OnEntityEvent, this, std::placeholders::_1));
}

//=================================================================================
void C_EditorLayer::OnUpdate()
{
	if (m_CurveEditing)
	{
		m_CurveEditing->Draw(m_dd);
		if (auto camera = m_Camera.lock())
			m_CurveEditing->OnUpdate(*camera.get(), m_Viewport);
	}
	if (m_AABBEditing)
	{
		m_AABBEditing->Draw(m_dd);
		if (auto camera = m_Camera.lock())
			m_AABBEditing->OnUpdate(*camera.get(), m_Viewport);
	}


	if (m_AnimationEditing)
		m_AnimationEditing->DrawGUI();

	if (m_EntityGizmo)
	{
		if (auto camera = m_Camera.lock())
		{
			C_MousePickingHelper mousePicking(m_Input, *camera.get(), m_Viewport);
			m_EntityGizmo->OnUpdate(*camera.get(), mousePicking);
			mousePicking.SelectInteraction();
		}
		m_EntityGizmo->Draw(m_dd);
		auto world = m_World.lock();
		if (world)
		{
			auto entity = world->GetEntity(m_SelectedEntity);
			if (auto basicEntity = std::dynamic_pointer_cast<Entity::C_BasicEntity>(entity))
			{
				const auto transform = basicEntity->GetModelMatrix();

				basicEntity->SetModelMatrix(glm::translate(transform, m_EntityGizmo->GetPositionDiff()));
			}
		}
	}
}

//=================================================================================
void C_EditorLayer::EditCurve(Renderer::C_Curve& curve)
{
	m_CurveEditing = std::make_unique<C_CurveEditor>(curve, m_Input);
}

//=================================================================================
void C_EditorLayer::EditAABB(Physics::Primitives::S_AABB& aabb)
{
	m_AABBEditing = std::make_unique<C_AABBEditor>(aabb, m_Input);
}

//=================================================================================
void C_EditorLayer::SetCamera(const std::shared_ptr<Renderer::I_CameraComponent>& camera)
{
	m_Camera = camera;
}

//=================================================================================
void C_EditorLayer::SetViewport(const Renderer::C_Viewport& viewport)
{
	m_Viewport = viewport;
}

//=================================================================================
void C_EditorLayer::SetWorld(std::shared_ptr<Entity::C_EntityManager> world)
{
	m_World = world;
	// entity removed anyway
	m_SelectedEntity = GUID::INVALID_GUID;
}

//=================================================================================
void C_EditorLayer::EditAnimation()
{
	m_AnimationEditing = std::make_unique<C_AnimationEditor>();
}

//=================================================================================
bool C_EditorLayer::OnEntityEvent(Core::C_EntityEvent& event)
{
	if (event.GetEventType() == Core::C_EntityEvent::EntityEvent::Selected)
	{
		// spawn gizmo

		// save selected entity
		m_SelectedEntity = event.GetEntity();
		if (m_SelectedEntity.isValid())
		{
			auto world = m_World.lock();
			if (world)
			{
				auto entity = world->GetEntity(m_SelectedEntity);
				m_EntityGizmo.emplace(entity->GetPosition(), m_Input);
			}
		}
		else
		{
			m_EntityGizmo.reset();
		}
	}
	if (event.GetEventType() == Core::C_EntityEvent::EntityEvent::LevelLoaded)
	{
		m_SelectedEntity = {};
		m_EntityGizmo.reset();
	}

	return false;
}

}; // namespace GLEngine::Editor