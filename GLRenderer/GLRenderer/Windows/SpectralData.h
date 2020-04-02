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

	GUI::Input::C_Slider<int>							m_Samples{100, 10, 401, "Sample count"};

	std::array<GUI::C_FormatedText, 6>		m_Results;
	GUI::Input::C_Button						m_UpdateResults;

	glm::mat3															m_XYZsRGB;

	enum class E_RadianceColors
	{
		E2,
		F4,
		G4,
		H4,
		J4,
		A1,
	};
	SpectralRendering::C_Spectrum m_Colors[6];
};

}
}