#include <GLRendererStdafx.h>

#include <GLRenderer/CameraManager.h>

#include <Renderer/ICameraComponent.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>
#include <GUI/Input/Button.h>

namespace GLEngine::GLRenderer::Temporar {

//=================================================================================
C_CameraManager::C_CameraManager()
	: Core::C_Layer("CameraManager")
	, m_ActiveCamera(nullptr)
	, m_DebugCamera(nullptr)
	, m_UseDebugCam(false, "Use debug cam")
	, m_Window(INVALID_GUID)
{

}

//=================================================================================
C_CameraManager::~C_CameraManager() = default;

//=================================================================================
void C_CameraManager::ActivateCamera(std::shared_ptr<Renderer::I_CameraComponent> camera)
{
	m_ActiveCamera = camera;
}

//=================================================================================
void C_CameraManager::SetDebugCamera(std::shared_ptr<Renderer::I_CameraComponent> camera)
{
	m_DebugCamera = camera;
}

//=================================================================================
std::shared_ptr<Renderer::I_CameraComponent> C_CameraManager::GetActiveCamera() const
{
	if (!m_UseDebugCam)
	{
		return m_ActiveCamera;
	}
	else
	{
		return m_DebugCamera;
	}
}

//=================================================================================
void C_CameraManager::OnEvent(Core::I_Event& event)
{
	if (!m_UseDebugCam)
	{
		m_ActiveCamera->OnEvent(event);
	}
	else
	{
		m_DebugCamera->OnEvent(event);
	}
}

//=================================================================================
GUID C_CameraManager::SetupControls(GUI::C_GUIManager& guiMGR)
{
	m_Window = guiMGR.CreateGUIWindow("Camera manager");
	auto* shaderMan = guiMGR.GetWindow(m_Window);

	shaderMan->AddComponent(m_UseDebugCam);

	return m_Window;
}

//=================================================================================
void C_CameraManager::DestroyControls(GUI::C_GUIManager& guiMGR)
{
	guiMGR.DestroyWindow(m_Window);
}

}