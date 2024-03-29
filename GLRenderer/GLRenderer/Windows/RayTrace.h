#pragma once

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/ProbeRenderer.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/Image.h>
#include <GUI/ImageViewer.h>
#include <GUI/Input/Button.h>
#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Slider.h>
#include <GUI/Input/Vector.h>

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_DebugDraw;
} // namespace GLEngine::Renderer

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
	void RayTraceProbe();

	bool IsRunning() const;

	virtual void			   RequestDestroy() override;
	[[nodiscard]] virtual bool CanDestroy() const override;
	void					   Update() override;

	std::shared_ptr<Textures::C_Texture> GetTexture() { return m_Probe; }
	glm::vec3							 GetProbePosition() { return m_ProbePosition.GetValue(); }

private:
	virtual void DrawComponents() const override;
	void		 UploadStorage();
	void		 SaveCurrentImage(const std::filesystem::path& texture);
	bool		 StillLoadingScene() const;
	void		 RecalculateHeatMap();
	//
	std::shared_ptr<Renderer::I_CameraComponent> m_Camera; // TODO: Should be weak? What should I do when camera moves?
	Textures::C_Texture							 m_Image;  // The presented result, TODO: make this pointer to base class or handle so I can move this to renderer or user code
	Textures::C_Texture							 m_HeatMap;
	Renderer::C_TextureViewStorageCPU<float>	 m_ImageStorage;   // Intermediate data, could need some weighting
	Renderer::C_TextureViewStorageCPU<float>	 m_SamplesStorage; // Intermediate data, could need some weighting
	Renderer::C_TextureViewStorageCPU<float>	 m_HeatMapStorage;
	Renderer::C_TextureViewStorageCPU<float>	 m_HeatMapNormalizedStorage;
	Renderer::C_RayTraceScene					 m_Scene;
	int											 m_NumCycleSamples;	 // How many samples had been already used per pixel, Used for sampling
	bool										 m_Running : 1;		 // Indicate that renderer is currently running
	bool										 m_RunningCycle : 1; // Run until stop feature
	GUI::Input::C_Slider<int>					 m_DepthSlider;		 // How many bounces should be traced
	std::unique_ptr<Renderer::C_RayRenderer>	 m_Renderer;
	std::unique_ptr<Renderer::C_ProbeRenderer>	 m_ProbeRenderer;
	std::shared_ptr<Textures::C_Texture>		 m_Probe;
	Renderer::C_TextureViewStorageCPU<float>	 m_ProbeStorage; // Intermediate data, could need some weighting
	GUI::C_ImageViewer							 m_GUIImage;
	GUI::C_Image								 m_GUIHeatMapImage;
	GUI::C_Image								 m_GUIImageProbe;
	GUI::Menu::C_Menu							 m_FileMenu;
	GUI::Input::C_CheckBoxValue					 m_DebugDraw;
	GUI::Input::C_Vec3							 m_ProbePosition;

	std::mutex m_ImageLock;
};
} // namespace GLEngine::GLRenderer
