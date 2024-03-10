#pragma once

#include <GLRenderer/CameraManager.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/GLFW/GLFWoGLWindow.h>
#include <GLRenderer/MainPassTechnique.h>
#include <GLRenderer/ShadowMapPass.h>
#include <GLRenderer/Textures/Texture.h>

#include <Renderer/Mesh/Curve.h>

#include <GUI/GUIWindow.h>
#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Slider.h>
#include <GUI/Menu/Menu.h>
#include <GUI/Menu/MenuItem.h>
#include <GUI/PlotLine.h>
#include <GUI/Text.h>

#include <Entity/EntityManager.h>

#include <Core/EventSystem/LayerStack.h>

#include <Utils/HighResolutionTimer.h>

#include <Editor/EditorLayer.h>

namespace GLEngine::Core {
class C_AppEvent;
class C_WindowResizedEvent;
} // namespace GLEngine::Core
namespace GLEngine::Renderer {
class I_CameraComponent;
}

namespace GLEngine::GLRenderer {
class C_GLImGUILayer;
class C_RayTraceWindow;
class C_Framebuffer;
class C_SunShadowMapTechnique;
class C_RenderInterface;

namespace Windows {
class C_ExplerimentWindow : public GLFW::C_GLFWoGLWindow {
	using T_Base = GLFW::C_GLFWoGLWindow;

public:
	explicit C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_ExplerimentWindow();
	//=================================================================================
	virtual void Update() override;

	//=================================================================================
	virtual void			   OnEvent(Core::I_Event& event) override;
	[[nodiscard]] virtual bool CanClose() const override;

protected:
	bool OnAppEvent(Core::C_AppEvent& event);
	bool OnWindowResized(Core::C_WindowResizedEvent& event);

private:
	void SetupWorld(const std::filesystem::path& level);
	void SaveLevel(const std::filesystem::path& filename);
	void SaveLevelAs();
	void AddMandatoryWorldParts();
	void OnAppInit();
	void MouseSelect();

	void sampleTime(double new_sample);

	std::shared_ptr<Entity::C_EntityManager> m_World;
	std::weak_ptr<Entity::I_Entity>			 m_Player;
	Core::C_LayerStack						 m_LayerStack;
	Temporar::C_CameraManager				 m_CamManager;
	C_GLImGUILayer*							 m_ImGUI;
	Utils::HighResolutionTimer				 m_FrameTimer;

	//===========================
	// GUI
	//===========================
	enum class E_GUITexts
	{
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
	GUID																m_EntitiesWindowGUID;
	GUID																m_HDRSettingsGUID;
	GUI::Menu::C_Menu													m_Windows;

	std::unique_ptr<C_MainPassTechnique>	 m_MainPass;
	std::shared_ptr<C_ShadowMapTechnique>	 m_ShadowPass;
	std::shared_ptr<C_SunShadowMapTechnique> m_SunShadow;

	std::unique_ptr<C_Framebuffer>	   m_HDRFBO;
	std::unique_ptr<C_Framebuffer>	   m_HDRFBOAtmosphere;
	std::unique_ptr<C_RenderInterface> m_RenderInterface;

	Editor::C_EditorLayer m_EditorLayer;
};

} // namespace Windows
} // namespace GLEngine::GLRenderer