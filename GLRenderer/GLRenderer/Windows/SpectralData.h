#pragma once

#include <GLRenderer/GLFW/GLFWoGLWindow.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/CameraManager.h>

#include <GLRenderer/GUI/GUIWindow.h>
#include <GLRenderer/GUI/ConsoleWindow.h>
#include <GLRenderer/GUI/PlotLine.h>
#include <GLRenderer/GUI/Input/Button.h>
#include <GLRenderer/GUI/Input/Slider.h>
#include <GLRenderer/GUI/Text.h>
#include <GLRenderer/GUI/GUIWindow.h>
#include <GLRenderer/GUI/Text.h>
#include <GLRenderer/GUI/Menu/Menu.h>
#include <GLRenderer/GUI/Menu/MenuItem.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>

#include <GLRenderer/SpectralRendering/SpectralPlot.h>

#include <GLRenderer/SpectralRendering/Spectrum.h>
#include <GLRenderer/SpectralRendering/MatchingFunction.h>

#include <Core/EventSystem/LayerStack.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Core {
	class C_AppEvent;
	class C_WindowResizedEvent;
}

namespace GLEngine::GLRenderer {
namespace ImGui {
	class C_ImGuiLayer;
}

namespace Windows {
class C_SpectralData : public GLFW::C_GLFWoGLWindow {
	using T_Base = GLFW::C_GLFWoGLWindow;
public:
	C_SpectralData(const Core::S_WindowInfo& wndInfo);
	virtual ~C_SpectralData();
	//=================================================================================
	virtual void Update() override;

	//=================================================================================
	virtual void OnEvent(Core::I_Event& event) override;

protected:
	bool OnAppInit(Core::C_AppEvent& event);

private:

	void UpdateResults();

	enum class E_Sampling
	{
		Uniform,
		Random,
		Hero,
	};
	enum class E_RadianceColors
	{
		E2,
		F4,
		G4,
		H4,
		J4,
		A1,
	};

	void SelectSampling(E_Sampling sampling);
	void SelectColor(E_RadianceColors color);

	void UpdateBackground();
	void UpdateSampledPlots(const int samples);

	Core::C_LayerStack																			m_LayerStack;
	ImGui::C_ImGuiLayer* m_ImGUI;

	SpectralRendering::C_Spectrum					m_ACIE;
	SpectralRendering::C_Spectrum					m_D65CIE;
	SpectralRendering::C_Spectrum					m_F11CIE;
	SpectralRendering::C_MatchingFunction m_MatchingFunction;
	SpectralRendering::C_SpectralPlot			m_LuminairePlot;
	SpectralRendering::C_SpectralPlot			m_MatchingPlot;
	SpectralRendering::C_SpectralPlot			m_ReflectancePlot;

	SpectralRendering::C_SpectralPlot			m_UniformlySampledPlot;
	SpectralRendering::C_SpectralPlot			m_RandomlySampledPlot;
	SpectralRendering::C_SpectralPlot			m_HeroSampledPlot;


	SpectralRendering::C_SpectralPlot			m_MultipliedReflLumi;
	SpectralRendering::C_SpectralPlot			m_MultipliedReflLumiXYZ;

	GUI::Input::C_Slider<int>							m_Samples{100, 10, 401, "Sample count"};

	std::array<GUI::C_FormatedText, 6>		m_Results;
	GUI::Input::C_Button									m_UpdateResults;

	std::array<GUI::Input::C_Button, 3>		m_SamplingButtons;
	std::array<GUI::Input::C_Button, 6>		m_ColorsButtons;

	glm::mat3															m_XYZsRGB;

	glm::vec3															m_ClearColor;

	using T_SamplerMethod = std::function<SpectralRendering::C_Spectrum(const SpectralRendering::C_Spectrum&)>;

	T_SamplerMethod												m_CurrentSampling;
	GUI::C_FormatedText										m_SelectedSampling;

	E_RadianceColors											m_SelectedColor;
	GUI::C_FormatedText										m_SelectedColorString;

	SpectralRendering::C_Spectrum m_Colors[6];
};

}
}