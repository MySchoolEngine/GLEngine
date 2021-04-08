#pragma once

#include <Core/EventSystem/Layer.h>

namespace GLEngine {
namespace Renderer {
class I_CameraComponent;
}

namespace GLRenderer { namespace Temporar {

class C_CameraManager : public Core::C_Layer {
public:
	C_CameraManager();
	virtual ~C_CameraManager() = default;

	void										 ActivateCamera(std::shared_ptr<Renderer::I_CameraComponent> camera);
	std::shared_ptr<Renderer::I_CameraComponent> GetActiveCamera() const;


	//==================================================
	// Core::C_Layer
	//==================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	std::shared_ptr<Renderer::I_CameraComponent> m_ActiveCamera;
};

}} // namespace GLRenderer::Temporar
} // namespace GLEngine
