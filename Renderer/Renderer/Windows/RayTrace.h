#pragma once

#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/ProbeRenderer.h>
#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/Image.h>
#include <GUI/ImageViewer.h>
#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Slider.h>
#include <GUI/Input/Vector.h>

namespace GLEngine::Entity {
class C_EntityManager;
}

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_DebugDraw;
class I_Renderer;

class RENDERER_API_EXPORT C_RayTraceWindow final : public GUI::C_Window {
public:
	C_RayTraceWindow(GUID guid, const std::shared_ptr<I_CameraComponent>& camera, GUI::C_GUIManager& guiMGR);
	~C_RayTraceWindow();

	void SetScene(Entity::C_EntityManager& world);

	void DebugDraw(I_DebugDraw* dd) const;

	void RayTrace();
	void Clear();
	void RunUntilStop();
	void StopAll();
	void RayTraceProbe();

	bool IsRunning() const;

	void			   RequestDestroy() override;
	[[nodiscard]] bool CanDestroy() const override;
	void			   Update() override;

	Handle<Texture> GetTexture() { return m_GPUProbeHandle; }
	glm::vec3		GetProbePosition() { return m_ProbePosition.GetValue(); }

private:
	void DrawComponents() const override;
	void UploadStorage();
	void SaveCurrentImage(const std::filesystem::path& texture);
	bool StillLoadingScene() const;
	void RecalculateHeatMap();
	void CreateTextures(I_Renderer& renderer);

	std::shared_ptr<I_CameraComponent> m_Camera; // TODO: Should be weak? What should I do when camera moves?
	Handle<Texture>					   m_GPUImageHandle;
	Handle<Texture>					   m_GPUHeatMapHandle;
	Handle<Texture>					   m_GPUProbeHandle;


	C_TextureViewStorageCPU<float>	 m_ImageStorage;   // Intermediate data, could need some weighting
	C_TextureViewStorageCPU<float>	 m_SamplesStorage; // Intermediate data, could need some weighting
	C_TextureViewStorageCPU<float>	 m_HeatMapStorage;
	C_TextureViewStorageCPU<float>	 m_HeatMapNormalizedStorage;
	C_RayTraceScene					 m_Scene;
	int								 m_NumCycleSamples;	 // How many samples had been already used per pixel, Used for sampling
	bool							 m_Running : 1;		 // Indicate that renderer is currently running
	bool							 m_RunningCycle : 1; // Run until stop feature
	GUI::Input::C_Slider<int>		 m_DepthSlider;		 // How many bounces should be traced
	std::unique_ptr<C_RayRenderer>	 m_Renderer;
	std::unique_ptr<C_ProbeRenderer> m_ProbeRenderer;
	// std::shared_ptr<Textures::C_Texture>		 m_Probe;
	C_TextureViewStorageCPU<float> m_ProbeStorage; // Intermediate data, could need some weighting
	GUI::C_ImageViewer			   m_GUIImage;
	GUI::C_Image				   m_GUIHeatMapImage;
	GUI::C_Image				   m_GUIImageProbe;
	GUI::Menu::C_Menu			   m_FileMenu;
	GUI::Input::C_CheckBoxValue	   m_DebugDraw;
	GUI::Input::C_Vec3			   m_ProbePosition;

	std::mutex m_ImageLock;
};
} // namespace GLEngine::Renderer
