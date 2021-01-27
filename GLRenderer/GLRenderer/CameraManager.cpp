#include <GLRendererStdafx.h>

#include <GLRenderer/CameraManager.h>

#include <Renderer/ICameraComponent.h>

namespace GLEngine::GLRenderer::Temporar {

//=================================================================================
C_CameraManager::C_CameraManager()
	: Core::C_Layer("CameraManager")
	, m_ActiveCamera(nullptr)
{
}

//=================================================================================
void C_CameraManager::ActivateCamera(std::shared_ptr<Renderer::I_CameraComponent> camera)
{
	m_ActiveCamera = camera;
}

//=================================================================================
std::shared_ptr<GLEngine::Renderer::I_CameraComponent> C_CameraManager::GetActiveCamera() const
{
	return m_ActiveCamera;
}

//=================================================================================
void C_CameraManager::OnEvent(Core::I_Event& event)
{
	m_ActiveCamera->OnEvent(event);
}

} // namespace GLEngine::GLRenderer::Temporar