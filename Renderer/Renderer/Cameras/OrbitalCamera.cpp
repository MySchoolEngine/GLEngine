#include <RendererStdafx.h>

#include <Renderer/Cameras/OrbitalCamera.h>

#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Ray.h>

#include <Core/Application.h>
#include <Core/EventSystem/Event/KeyboardEvents.h>
#include <Core/EventSystem/Event/MouseEvents.h>
#include <Core/EventSystem/EventDispatcher.h>
#include <Core/IWindow.h>
#include <Core/IWindowManager.h>
#include <Core/Input.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/transform.hpp>

// TODO: here only for key codes.
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace GLEngine::Renderer::Cameras {

//=================================================================================
C_OrbitalCamera::C_OrbitalCamera(std::shared_ptr<Entity::I_Entity> &owner)
    : I_CameraComponent(owner), m_ControlSpeed(0.5f),
      GLE_DEBUG_MEMBER_CTOR_LIST((0.0f, -89.0f, 89.0f, "Y angle:"), (0.0f),
                                 _angleYDeg),
      GLE_DEBUG_MEMBER_CTOR_LIST((0.0f, 0, 360.f, "X angle:"), (0.0f),
                                 _angleXDeg),
      GLE_DEBUG_MEMBER_CTOR_LIST((0.0f, 0.1f, 50.f, "Zoom:"), (0.0f), _zoom) {
  _pos = _view = _up = _left = glm::vec3(0);
}

//=================================================================================
C_OrbitalCamera::~C_OrbitalCamera() = default;

//=================================================================================
void C_OrbitalCamera::setupCameraView(float zoom, glm::vec3 center,
                                      float angleXDeg, float angleYDeg) {
  _zoom = zoom;

  _center = center;
  _angleXDeg = angleXDeg;
  _angleYDeg = angleYDeg;
}

//=================================================================================
void C_OrbitalCamera::setupCameraProjection(float nearZ, float farZ,
                                            float aspectRatio, float fovY) {
  _nearZ = (nearZ);
  _farZ = (farZ);
  _aspect = aspectRatio;
  _fovy = glm::radians(fovY);
}

//=================================================================================
void C_OrbitalCamera::adjustOrientation(float dx, float dy) {
  _angleXDeg += dx;
  _angleYDeg += dy;
  _angleYDeg =
      glm::max(glm::min(static_cast<float>(_angleYDeg), 89.0f), -89.0f);
}

//=================================================================================
void C_OrbitalCamera::adjustZoom(int d) {
  float c = 1.1f;
  _zoom = d < 0 ? _zoom * c : _zoom / c;
}

//=================================================================================
void C_OrbitalCamera::setCenterPoint(const glm::vec3 &center) {
  _center = center;
}

//=================================================================================
void C_OrbitalCamera::DebugDraw() {}

//=================================================================================
void C_OrbitalCamera::Update() {
  float radx = glm::radians(static_cast<float>(_angleXDeg));
  float rady = glm::radians(static_cast<float>(_angleYDeg));

  float x = _zoom * cos(rady) * cos(radx);
  float y = _zoom * sin(rady);
  float z = _zoom * cos(rady) * sin(radx);

  _pos = glm::vec3(x, y, z) + _center;
  _view = _center - _pos;
  _left = glm::cross(glm::vec3(0, 1, 0), glm::normalize(_view));
  _up = glm::cross(_view, _left);

  /*
  _pos = glm::vec3(x, y, z);
  _pos += _center;
  _up = glm::vec3(0, 1, 0);
  */
  _viewMatrix = glm::lookAt(_pos, _center, _up);
  _projectionMatrix = glm::perspective(_fovy, _aspect, GetNear(), GetFar());
  _ScreenToWorld = glm::inverse(_projectionMatrix * _viewMatrix);
}

//=================================================================================
float C_OrbitalCamera::GetFov() const { return _fovy; }

//=================================================================================
void C_OrbitalCamera::OnEvent(Core::I_Event &event) {
  Core::C_EventDispatcher d(event);
  d.Dispatch<Core::C_KeyPressedEvent>(
      std::bind(&C_OrbitalCamera::OnKeyPressed, this, std::placeholders::_1));
  d.Dispatch<Core::C_KeyRepeatedEvent>(
      std::bind(&C_OrbitalCamera::OnKeyRepeated, this, std::placeholders::_1));
  d.Dispatch<Core::C_MouseScrollEvent>(
      std::bind(&C_OrbitalCamera::OnMouseScroll, this, std::placeholders::_1));
  d.Dispatch<Core::C_MouseButtonPressed>(
      std::bind(&C_OrbitalCamera::OnMousePress, this, std::placeholders::_1));
}

//=================================================================================
void C_OrbitalCamera::DebugDrawGUI() {
#ifdef GL_ENGINE_DEBUG
  _angleYDeg.Draw();
  _angleXDeg.Draw();
  _zoom.Draw();
#endif
}

//=================================================================================
std::string_view C_OrbitalCamera::GetDebugComponentName() const {
  return "Orbital Camera";
}

//=================================================================================
bool C_OrbitalCamera::HasDebugDrawGUI() const { return true; }

//=================================================================================
bool C_OrbitalCamera::OnKeyEvent(Core::C_KeyEvent &event) {
  //===============================================
  // Rotations
  if (event.GetKeyCode() == GLFW_KEY_DOWN) {
    adjustOrientation(0.0f, -m_ControlSpeed);
    Update();
    return true;
  }
  if (event.GetKeyCode() == GLFW_KEY_UP) {
    adjustOrientation(0.0f, m_ControlSpeed);
    return true;
  }
  if (event.GetKeyCode() == GLFW_KEY_LEFT) {
    adjustOrientation(m_ControlSpeed, 0.0f);
    return true;
  }
  if (event.GetKeyCode() == GLFW_KEY_RIGHT) {
    adjustOrientation(-m_ControlSpeed, 0.0f);
    return true;
  }

  //===============================================
  // Position
  glm::vec3 normalToPlane = glm::vec3(0.0f, 1.0f, 0.0f);
  glm::vec3 forwardProjOnXZ =
      glm::normalize(_view - glm::proj(_view, normalToPlane)) * m_ControlSpeed;
  glm::vec4 left = glm::rotate(glm::vec4(forwardProjOnXZ, 1.0f),
                               -glm::half_pi<float>(), normalToPlane);
  glm::vec3 leftProjOnXZ = glm::vec3(left.x, 0.0f, left.z);
  if (event.GetKeyCode() == GLFW_KEY_W) {
    _center += forwardProjOnXZ;
    return true;
  }
  if (event.GetKeyCode() == GLFW_KEY_S) {
    _center -= forwardProjOnXZ;
    return true;
  }
  if (event.GetKeyCode() == GLFW_KEY_A) {
    _center -= leftProjOnXZ;
    return true;
  }
  if (event.GetKeyCode() == GLFW_KEY_D) {
    _center += leftProjOnXZ;
    return true;
  }
  return false;
}

//=================================================================================
bool C_OrbitalCamera::OnKeyPressed(Core::C_KeyPressedEvent &event) {
  if (OnKeyEvent(event)) {
    return true;
  }

  //===============================================
  // Zoom
  if (event.GetKeyCode() == GLFW_KEY_MINUS ||
      event.GetKeyCode() == GLFW_KEY_KP_SUBTRACT) {
    adjustZoom(-2);
    return true;
  }
  if (event.GetKeyCode() == GLFW_KEY_KP_ADD) {
    adjustZoom(+2);
    return true;
  }
  return false;
}

//=================================================================================
bool C_OrbitalCamera::OnKeyRepeated(Core::C_KeyRepeatedEvent &event) {
  if (OnKeyEvent(event)) {
    return true;
  }
  return false;
}

//=================================================================================
bool C_OrbitalCamera::OnMouseScroll(Core::C_MouseScrollEvent &event) {
  adjustZoom(static_cast<int>(-event.GetYOffset() * 10));
  return true;
}

//=================================================================================
bool C_OrbitalCamera::OnMousePress(Core::C_MouseButtonPressed &event) {
  if (event.GetMouseButton() == 0) {
    const auto window =
        Core::C_Application::Get().GetWndMgr().GetWindow(event.GetWindowGUID());
    const auto screenCoord = window->GetInput().GetMousePosition();

    const auto clipPosition =
        window->ToClipSpace({screenCoord.first, screenCoord.second});

    const Physics::Primitives::S_Ray ray = GetRay(clipPosition);
    constexpr Physics::Primitives::S_Plane ground{
        glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.f};

    const auto intersect = ground.IntersectImpl(ray);

    if (intersect > 0) {
      const glm::vec4 hit =
          glm::vec4(_pos, 1.0f) + glm::vec4(ray.direction, 1.0f) * intersect;

      _center = hit;
      Update();
      return true;
    }
  }
  return false;
}

//=================================================================================
float C_OrbitalCamera::GetAspectRatio() const { return _aspect; }

//=================================================================================
glm::quat C_OrbitalCamera::GetRotation() const {
  throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
glm::vec3 C_OrbitalCamera::GetDirection() const {
  return _center - GetPosition();
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetViewProjectionMatrix() const {
  return _projectionMatrix * _viewMatrix;
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetProjectionMatrix() const {
  return _projectionMatrix;
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetViewMatrix() const { return _viewMatrix; }

//=================================================================================
glm::vec3 C_OrbitalCamera::GetPosition() const { return _pos; }

//=================================================================================
Physics::Primitives::C_Frustum C_OrbitalCamera::GetFrustum() const {
  return Physics::Primitives::C_Frustum(GetPosition(), _up, GetDirection(),
                                        GetNear(), GetFar(), GetAspectRatio(),
                                        GetFov());
}

//=================================================================================
glm::mat4 C_OrbitalCamera::GetScreenToworldMatrix() const {
  return _ScreenToWorld;
}

} // namespace GLEngine::Renderer::Cameras