#pragma once

#include <GLRenderer/CameraManager.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/GLFW/GLFWoGLWindow.h>
#include <GLRenderer/MainPassTechnique.h>
#include <GLRenderer/ShadowMapPass.h>

#include <Renderer/Renderer2D.h>
#include <Renderer/Renderer3D.h>

#include <Editor/EditorLayer.h>

#include <GUI/GUIWindow.h>
#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Slider.h>
#include <GUI/Menu/Menu.h>
#include <GUI/PlotLine.h>
#include <GUI/Text.h>

#include <Entity/EntityManager.h>

#include <Core/EventSystem/LayerStack.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Core {
class C_AppEvent;
class C_WindowResizedEvent;
} // namespace GLEngine::Core
namespace GLEngine::Renderer {
class I_CameraComponent;
class C_StaticMeshHandles;
class C_RayTraceWindow;
} // namespace GLEngine::Renderer

namespace GLEngine::GLRenderer {
class C_GLImGUILayer;
class C_Framebuffer;
class C_SunShadowMapTechnique;
class C_RenderInterface;
class C_GLRenderInterface3D;
class C_GLRendererInterface2D;

namespace Windows {
class C_ExperimentWindow : public GLFW::C_GLFWoGLWindow {
	using T_Base = GLFW::C_GLFWoGLWindow;

public:
	explicit C_ExperimentWindow(const Core::S_WindowInfo& wndInfo);
	~C_ExperimentWindow() override;
	//=================================================================================
	void Update() override;

	//=================================================================================
	void			   OnEvent(Core::I_Event& event) override;
	[[nodiscard]] bool CanClose() const override;

protected:
	bool OnAppEvent(Core::C_AppEvent& event);
	bool OnWindowResized(Core::C_WindowResizedEvent& event);

private:
	void SetupWorld(const std::filesystem::path& level);
	void SaveLevel(const std::filesystem::path& filename);
	bool SaveLevelAs();
	void AddMandatoryWorldParts();
	void OnAppInit();
	void MouseSelect();

	void SampleTime(double newSample);

	std::shared_ptr<Entity::C_EntityManager> m_World;
	std::weak_ptr<Entity::I_Entity>			 m_Player;
	Core::C_LayerStack						 m_LayerStack;
	Temporar::C_CameraManager				 m_CamManager;
	C_GLImGUILayer*							 m_ImGUI;
	::Utils::HighResolutionTimer			 m_FrameTimer;

	//===========================
	// GUI
	//===========================
	enum class E_GUITexts : std::uint8_t {
		AvgFrametime,
		AvgFps,
		MinMaxFrametime,
		Last,
	};
	GUI::C_PlotLine<500>												m_Samples;
	GUI::Input::C_Slider<float>											m_GammaSlider;
	GUI::Input::C_Slider<float>											m_ExposureSlider;
	GUI::Input::C_CheckBoxValue											m_VSync;
	std::array<GUI::C_FormatedText, static_cast<int>(E_GUITexts::Last)> m_GUITexts;
	GUID																m_FrameStatsGUID;
	GUID																m_ConsoleWindowGUID;
	GUID																m_RayTraceGUID;
	GUID																m_WaterRenderingGUID;
	GUID																m_ImageEditorGUID;
	GUID																m_EntityEditorGUID;
	GUID																m_EntitiesWindowGUID;
	GUID																m_HDRSettingsGUID;
	GUI::Menu::C_Menu													m_Windows;

	std::unique_ptr<C_MainPassTechnique>	 m_MainPass;
	std::shared_ptr<C_ShadowMapTechnique>	 m_ShadowPass;
	std::shared_ptr<C_SunShadowMapTechnique> m_SunShadow;

	std::unique_ptr<C_Framebuffer>			 m_HDRFBO;
	std::unique_ptr<C_Framebuffer>			 m_HDRFBOAtmosphere;
	std::unique_ptr<C_RenderInterface>		 m_RenderInterface;
	std::unique_ptr<C_GLRenderInterface3D>	 m_RenderInterfaceHandles;
	std::unique_ptr<C_GLRendererInterface2D> m_2DRenderInterfaceHandles;

	Editor::C_EditorLayer m_EditorLayer;

	Renderer::Renderer3D   m_3DRenderer;
	Renderer::C_Renderer2D m_2DRenderer;
};

} // namespace Windows
} // namespace GLEngine::GLRenderer