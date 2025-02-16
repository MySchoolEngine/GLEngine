#include <RendererStdafx.h>

#include <Renderer/Cameras/OrbitalCamera.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/Primitives/Frustum.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Ray.h>

#include <Entity/BasicEntity.h>

#include <Core/Application.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/IWindow.h>
#include <Core/IWindowManager.h>
#include <Core/Input.h>

#include <Utils/Reflection/Metadata.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

#include <rttr/registration>

// TODO: here only for key codes.
#include <GLFW/glfw3.h>
#include <stdexcept>

#pragma region registration
// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer::Cameras;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_OrbitalCamera>("C_OrbitalCamera")
		.constructor<std::shared_ptr<GLEngine::Entity::I_Entity>>()(rttr::policy::ctor::as_std_shared_ptr)
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.method("AfterDeserialize", &C_OrbitalCamera::AfterDeserialize)
		.property("nearZ", &C_OrbitalCamera::_nearZ)
		.property("farZ", &C_OrbitalCamera::_farZ)
		.property("aspectZ", &C_OrbitalCamera::_aspect)
		.property("YAngle", &C_OrbitalCamera::_angleYRad)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Angle>(),
				RegisterMetamember<UI::Angle::Name>("Y angle:"),
				RegisterMetamember<UI::Angle::Min>(-89.0f),
				RegisterMetamember<UI::Angle::Max>(89.0f)
			 )
		.property("XAngle", &C_OrbitalCamera::_angleXRad)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Angle>(),
				RegisterMetamember<UI::Angle::Name>("X angle:"),
				RegisterMetamember<UI::Angle::Min>(0.0f),
				RegisterMetamember<UI::Angle::Max>(360.0f)
			)
		.property("Zoom", &C_OrbitalCamera::_zoom)
			(
				rttr::policy::prop::as_reference_wrapper, 
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Zoom:"),
				RegisterMetamember<UI::Slider::Min>(0.1f),
				RegisterMetamember<UI::Slider::Max>(50.0f)
			);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_OrbitalCamera>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_OrbitalCamera> ptr, bool& ok)
	  -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}
// clang-format on
#pragma endregion

namespace GLEngine::Renderer::Cameras {

//=================================================================================
C_OrbitalCamera::C_OrbitalCamera(const std::shared_ptr<Entity::I_Entity>& owner)
	: I_CameraComponent(owner)
	, _zoom()
	, _angleXRad(0.0f)
	, _angleYRad(0.0f)
	, m_ControlSpeed(glm::radians(0.5f))
{
	_pos = _view = _up = _left = glm::vec3(0);
	m_Transformation.SetEnabledTransforms(GUI::Input::C_Transformations::E_Transorms::Translate);
}

//=================================================================================
C_OrbitalCamera::C_OrbitalCamera()
	: I_CameraComponent(nullptr)
	, _fovy(glm::radians(90.f))
	, _nearZ(0.1f)
	, _farZ(100.f)
	, _aspect(1.85546875f)
	, _zoom(5.0f)
	, _angleXRad(glm::half_pi<float>())
	, _angleYRad(0.f)
	, m_ControlSpeed(glm::radians(0.5f))
{
}

//=================================================================================
C_OrbitalCamera::~C_OrbitalCamera() = default;

//=================================================================================
void C_OrbitalCamera::SetupCameraView(const float zoom, const glm::vec3 center, const float angleXDeg, const float angleYDeg)
{
	_zoom = zoom;

	m_Transformation.SetTranslation(center);
	_angleXRad = glm::radians(angleXDeg);
	_angleYRad = glm::radians(angleYDeg);
}

//=================================================================================
void C_OrbitalCamera::SetupCameraProjection(float nearZ, float farZ, float aspectRatio, float fovY)
{
	_nearZ	= (nearZ);
	_farZ	= (farZ);
	_aspect = aspectRatio;
	_fovy	= glm::radians(fovY);
}

//=================================================================================
void C_OrbitalCamera::AdjustOrientation(float dx, float dy)
{
	_angleXRad += dx;
	_angleYRad += dy;
	_angleYRad = glm::max(glm::min(_angleYRad, glm::half_pi<float>()), -glm::half_pi<float>());
}

//=================================================================================
void C_OrbitalCamera::AdjustZoom(int d)
{
	float c = 1.1f;
	_zoom	= d < 0 ? _zoom * c : _zoom / c;
}

//=================================================================================
void C_OrbitalCamera::DebugDraw()
{
}

//=================================================================================
void C_OrbitalCamera::Update()
{
	const float x = _zoom * cos(_angleYRad) * cos(_angleXRad);
	const float y = _zoom * sin(_angleYRad);
	const float z = _zoom * cos(_angleYRad) * sin(_angleXRad);

	const auto centerOfView = m_Transformation.GetTranslation();

	_pos  = glm::vec3(x, y, z) + centerOfView;
	_view = centerOfView - _pos;
	_left = glm::cross(glm::vec3(0, 1, 0), glm::normalize(_view));
	_up	  = glm::cross(_view, _left);

	_viewMatrix		  = glm::lookAt(_pos, centerOfView, _up);
	_projectionMatrix = glm::perspective(_fovy, _aspect, GetNear(), GetFar());
	_ScreenToWorld	  = glm::inverse(_projectionMatrix * _viewMatrix);
}

//=================================================================================
float C_OrbitalCamera::GetFov() const
{
	return _fovy;
}

//=================================================================================
void C_OrbitalCamera::OnEvent(Core::I_Event& event)
{
	Core::C_EventDispatcher d(event);
	d.Dispatch<Core::C_KeyPressedEvent>(std::bind(&C_OrbitalCamera::OnKeyPressed, this, std::placeholders::_1));
	d.Dispatch<Core::C_KeyRepeatedEvent>(std::bind(&C_OrbitalCamera::OnKeyRepeated, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseScrollEvent>(std::bind(&C_OrbitalCamera::OnMouseScroll, this, std::placeholders::_1));
	d.Dispatch<Core::C_MouseButtonPressed>(std::bind(&C_OrbitalCamera::OnMousePress, this, std::placeholders::_1));
}

//=================================================================================
void C_OrbitalCamera::DebugDrawGUI()
{
	rttr::instance obj(*this);
	GUI::DrawAllPropertyGUI(obj);
}

//=================================================================================
std::string_view C_OrbitalCamera::GetDebugComponentName() const
{
	return "Orbital Camera";
}

//=================================================================================
bool C_OrbitalCamera::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
bool C_OrbitalCamera::OnKeyEvent(Core::C_KeyEvent& event)
{
	//===============================================
	// Rotations
	if (event.GetKeyCode() == GLFW_KEY_DOWN)
	{
		AdjustOrientation(0.0f, -m_ControlSpeed);
		Update();
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_UP)
	{
		AdjustOrientation(0.0f, m_ControlSpeed);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_LEFT)
	{
		AdjustOrientation(m_ControlSpeed, 0.0f);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_RIGHT)
	{
		AdjustOrientation(-m_ControlSpeed, 0.0f);
		return true;
	}

	//===============================================
	// Position
	glm::vec3 normalToPlane	  = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 forwardProjOnXZ = glm::normalize(_view - glm::proj(_view, normalToPlane)) * m_ControlSpeed;
	glm::vec4 left			  = glm::rotate(glm::vec4(forwardProjOnXZ, 1.0f), -glm::half_pi<float>(), normalToPlane);
	glm::vec3 leftProjOnXZ	  = glm::vec3(left.x, 0.0f, left.z);
	if (event.GetKeyCode() == GLFW_KEY_W)
	{
		m_Transformation.SetTranslation(m_Transformation.GetTranslation() + forwardProjOnXZ);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_S)
	{
		m_Transformation.SetTranslation(m_Transformation.GetTranslation() - forwardProjOnXZ);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_A)
	{
		m_Transformation.SetTranslation(m_Transformation.GetTranslation() - leftProjOnXZ);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_D)
	{
		m_Transformation.SetTranslation(m_Transformation.GetTranslation() + leftProjOnXZ);
		return true;
	}
	return false;
}

//=================================================================================
bool C_OrbitalCamera::OnKeyPressed(Core::C_KeyPressedEvent& event)
{
	if (OnKeyEvent(event))
	{
		return true;
	}

	//===============================================
	// Zoom
	if (event.GetKeyCode() == GLFW_KEY_MINUS || event.GetKeyCode() == GLFW_KEY_KP_SUBTRACT)
	{
		AdjustZoom(-2);
		return true;
	}
	if (event.GetKeyCode() == GLFW_KEY_KP_ADD)
	{
		AdjustZoom(+2);
		return true;
	}
	return false;
}

//=================================================================================
bool C_OrbitalCamera::OnKeyRepeated(Core::C_KeyRepeatedEvent& event)
{
	if (OnKeyEvent(event))
	{
		return true;
	}
	return false;
}

//=================================================================================
bool C_OrbitalCamera::OnMouseScroll(Core::C_MouseScrollEvent& event)
{
	AdjustZoom(static_cast<int>(-event.GetYOffset() * 10));
	return true;
}

//=================================================================================
bool C_OrbitalCamera::OnMousePress(Core::C_MouseButtonPressed& event)
{
	if (event.GetMouseButton() == 0)
	{
		const auto window	   = Core::C_Application::Get().GetWndMgr().GetWindow(event.GetWindowGUID());
		const auto screenCoord = window->GetInput().GetMousePosition();

		const auto clipPosition = window->ToClipSpace({screenCoord.first, screenCoord.second});

		const Physics::Primitives::S_Ray	   ray = GetRay(clipPosition);
		constexpr Physics::Primitives::S_Plane ground{glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.f};

		const auto intersect = ground.IntersectImpl(ray);

		if (intersect > 0)
		{
			const glm::vec4 hit = glm::vec4(_pos, 1.0f) + glm::vec4(ray.direction, 1.0f) * intersect;

			m_Transformation.SetTranslation(hit);
			Update();
			return true;
		}
	}
	return false;
}

//=================================================================================
float C_OrbitalCamera::GetAspectRatio() const
{
	return _aspect;
}

//=================================================================================
glm::quat C_OrbitalCamera::GetRotation() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
glm::vec3 C_OrbitalCamera::GetDirection() const
{
	return m_Transformation.GetTranslation() - GetPosition();
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetViewProjectionMatrix() const
{
	return _projectionMatrix * _viewMatrix;
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetProjectionMatrix() const
{
	return _projectionMatrix;
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetViewMatrix() const
{
	return _viewMatrix;
}

//=================================================================================
glm::vec3 C_OrbitalCamera::GetPosition() const
{
	return _pos;
}

//=================================================================================
Physics::Primitives::C_Frustum C_OrbitalCamera::GetFrustum() const
{
	return Physics::Primitives::C_Frustum(GetPosition(), _up, GetDirection(), GetNear(), GetFar(), GetAspectRatio(), GetFov());
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetScreenToWorldMatrix() const
{
	return _ScreenToWorld;
}

//=================================================================================
void C_OrbitalCamera::AfterDeserialize()
{
	Update();
}

} // namespace GLEngine::Renderer::Cameras