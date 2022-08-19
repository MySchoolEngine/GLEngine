#pragma once

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Textures/TextureStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/Image.h>
#include <GUI/Input/Button.h>
#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Slider.h>

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_DebugDraw;
}

namespace GLEngine::GLRenderer {
namespace Textures {
class C_Texture;
}
class C_RayTraceWindow : public GUI::C_Window {
public:
	C_RayTraceWindow(GUID guid, std::shared_ptr<Renderer::I_CameraComponent> camera, GUI::C_GUIManager& guiMGR);
	~C_RayTraceWindow();

	void DebugDraw(Renderer::I_DebugDraw* dd) const;

	void RayTrace();
	void Clear();
	void RunUntilStop();
	void StopAll();

	bool IsRunning() const;

	virtual void			   RequestDestroy() override;
	[[nodiscard]] virtual bool CanDestroy() const override;

private:
	virtual void DrawComponents() const override;
	void		 UploadStorage();
	void		 SaveCurrentImage(const std::filesystem::path& texture);

	std::shared_ptr<Renderer::I_CameraComponent> m_Camera; // TODO: Should be weak? What should I do when camera moves?
	Textures::C_Texture							 m_Image;  // The presented result, TODO: make this pointer to base class or handle so I can move this to renderer or user code
	Renderer::C_TextureViewStorageCPU<float>	 m_ImageStorage;   // Intermediate data, could need some weighting
	Renderer::C_TextureViewStorageCPU<float>	 m_SamplesStorage; // Intermediate data, could need some weighting
	Renderer::C_RayTraceScene					 m_Scene;
	std::future<void>							 m_SignalDone;		 // Signaling that work has ended, no one uses that?
	int											 m_NumCycleSamples;	 // How many samples had been already used per pixel, Used for sampling
	bool										 m_Running : 1;		 // Indicate that renderer is currntly running
	bool										 m_RunningCycle : 1; // Run until stop feature
	GUI::Input::C_Slider<int>					 m_DepthSlider;		 // How many bounces should be traced
	Renderer::C_RayRenderer						 m_Renderer;
	GUI::C_Image								 m_GUIImage;
	GUI::Menu::C_Menu							 m_FileMenu;
	GUI::Input::C_CheckBoxValue					 m_DebugDraw;

	std::mutex m_ImageLock;
};
} // namespace GLEngine::GLRenderer
