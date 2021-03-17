#pragma once

#include <GUI/Input/CheckBoxValue.h>

#include <Core/EventSystem/Layer.h>

namespace GLEngine {
namespace GUI {
class C_GUIManager;
}
namespace Renderer {
class I_CameraComponent;
}

namespace GLRenderer::Temporar {

class C_CameraManager : public Core::C_Layer {
public:
	C_CameraManager();
	virtual ~C_CameraManager(); // = default

	void										 ActivateCamera(std::shared_ptr<Renderer::I_CameraComponent> camera);
	void										 SetDebugCamera(std::shared_ptr<Renderer::I_CameraComponent> camera);
	std::shared_ptr<Renderer::I_CameraComponent> GetActiveCamera() const;
	std::shared_ptr<Renderer::I_CameraComponent> GetMainCamera() const { return m_ActiveCamera; }


	//==================================================
	// Core::C_Layer
	//==================================================
	virtual void OnEvent(Core::I_Event& event) override;

	GUID SetupControls(GUI::C_GUIManager& guiMGR);
	void DestroyControls(GUI::C_GUIManager& guiMGR);

protected:
	std::shared_ptr<Renderer::I_CameraComponent> m_ActiveCamera;
	std::shared_ptr<Renderer::I_CameraComponent> m_DebugCamera;
	GUID										 m_Window;

private:
	GUI::Input::C_CheckBoxValue m_UseDebugCam;
};

} // namespace GLRenderer::Temporar
} // namespace GLEngine
