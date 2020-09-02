#pragma once

#include <GLRenderer/GLFW/GLFWoGLWindow.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/CameraManager.h>

#include <GUI/GUIWindow.h>
#include <GUI/PlotLine.h>
#include <GUI/Input/CheckBoxValue.h>
#include <GUI/Input/Slider.h>
#include <GUI/GUIWindow.h>
#include <GUI/Text.h>
#include <GUI/Menu/Menu.h>
#include <GUI/Menu/MenuItem.h>
#include <GLRenderer/MainPassTechnique.h>
#include <GLRenderer/ShadowMapPass.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>

#include <Entity/EntityManager.h>

#include <Core/EventSystem/LayerStack.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Core {
class C_AppEvent;
class C_WindowResizedEvent;
}
namespace GLEngine::Renderer {
class I_CameraComponent;
}

namespace GLEngine::GLRenderer {
namespace ImGui {
class C_ImGuiLayer;
}

namespace Components {
class C_StaticMesh;
}

class C_Framebuffer;

namespace Windows {
class C_ExplerimentWindow : public GLFW::C_GLFWoGLWindow {
	using T_Base = GLFW::C_GLFWoGLWindow;
public:
	explicit C_ExplerimentWindow(const Core::S_WindowInfo& wndInfo);
	virtual ~C_ExplerimentWindow();
	//=================================================================================
	virtual void Update() override;

	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	bool OnKeyPressed(Core::C_KeyPressedEvent& event);
	bool OnAppInit(Core::C_AppEvent& event);
	bool OnWindowResized(Core::C_WindowResizedEvent& event);

private:
	void SetupWorld();
	void MouseSelect();

	void sampleTime(double new_sample);

	std::shared_ptr<Entity::C_EntityManager>								m_World;
	std::weak_ptr<Entity::I_Entity>													m_Player;
	Core::C_LayerStack																			m_LayerStack;
	Temporar::C_CameraManager																m_CamManager;
	ImGui::C_ImGuiLayer*																		m_ImGUI;
	Utils::HighResolutionTimer															m_FrameTimer;

	//===========================
	// GUI
	//===========================
	enum class E_GUITexts {
		AvgFrametime,
		AvgFps,
		MinMaxFrametime,
		Last,
	};
	GUI::C_PlotLine<500>																		m_Samples;
	GUI::Input::C_Slider<float>															m_GammaSlider;
	GUI::Input::C_Slider<float>															m_ExposureSlider;
	GUI::Input::C_CheckBoxValue															m_VSync;
	std::array<GUI::C_FormatedText, static_cast<int>(E_GUITexts::Last)>				m_GUITexts;
	GUID																										m_FrameStatsGUID;
	GUID																										m_ConsoleWindowGUID;
	GUID																										m_EntitiesWindowGUID;
	GUID																										m_HDRSettingsGUID;
	GUI::Menu::C_Menu																				m_Windows;

	std::unique_ptr<C_MainPassTechnique>																		m_MainPass;
	std::shared_ptr<C_ShadowMapTechnique>										m_ShadowPass;

	std::unique_ptr <C_Framebuffer>																						m_HDRFBO;
	std::shared_ptr<Mesh::C_StaticMeshResource>							m_ScreenQuad;
};

}}