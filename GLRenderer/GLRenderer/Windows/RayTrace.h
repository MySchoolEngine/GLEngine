#pragma once

#include <GUI/GUIWindow.h>
#include <GUI/Input/Button.h>
#include <GUI/Input/CheckBoxValue.h>


#include <Renderer/Textures/TextureStorage.h>
#include <Renderer/RayCasting/RayTraceScene.h>

namespace GLEngine::Renderer {
class I_CameraComponent;
}

namespace GLEngine::GLRenderer {
namespace Textures {
class C_Texture;
}
class C_RayTraceWindow : public GUI::C_Window
{
public:
	C_RayTraceWindow(GUID guid, std::shared_ptr<Renderer::I_CameraComponent> camera);


	void RayTrace();
	void Clear();
	void RunUntilStop();
	void StopAll();

	bool IsRunning() const;

private:
	virtual void DrawComponents() const override;
	void UploadStorage() const;

	std::shared_ptr<Renderer::I_CameraComponent>		m_Camera;
	std::shared_ptr<Textures::C_Texture>						m_Image;
	Renderer::C_TextureViewStorageCPU<float>				m_ImageStorage;
	Renderer::C_RayTraceScene												m_Scene;
	std::future<void>																m_SignalDone;
	int																							m_NumCycleSamples;
	bool																						m_Running			: 1;
	bool																						m_RunningCycle: 1;
	GUI::Input::C_CheckBoxValue											m_LiveUpdate;
};
}

