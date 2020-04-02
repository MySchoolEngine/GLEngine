#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/SpectralData.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>

#include <GLRenderer/ImGui/ImGuiLayer.h>

#include <GLRenderer/OGLRenderer.h>

#include <GLRenderer/SpectralRendering/Dataparser.h>

namespace GLEngine::GLRenderer::Windows {

//=================================================================================
C_SpectralData::C_SpectralData(const Core::S_WindowInfo& wndInfo)
	: C_GLFWoGLWindow(wndInfo)
	, m_LayerStack(std::string("SpectralDataLayerStack"))
	, m_LuminairePlot("Luminare")
	, m_MatchingPlot("Matching function")
	, m_ReflectancePlot("Reflectance")
	, m_UniformlySampledPlot("Sampled")
	, m_RandomlySampledPlot("Random")
	, m_HeroSampledPlot("Hero")
	, m_MultipliedReflLumi("Multiplied")
{
	glfwMakeContextCurrent(m_Window);


	m_ImGUI = new ImGui::C_ImGuiLayer(m_ID);
	m_ImGUI->OnAttach(); // manual call for now.
	m_LayerStack.PushLayer(m_ImGUI);

	m_LuminairePlot.m_Color.resize(3);
	m_MatchingPlot.m_Color.resize(3);
	m_ReflectancePlot.m_Color.resize(6);
	m_LuminairePlot.m_Color[0] = glm::vec3(1, 0, 0);
	m_LuminairePlot.m_Color[1] = glm::vec3(0, 0, 1);
	m_LuminairePlot.m_Color[2] = glm::vec3(0.6, 0.6, 0.6);
	m_MatchingPlot.m_Color[0] = glm::vec3(1, 0, 0);
	m_MatchingPlot.m_Color[1] = glm::vec3(0, 1, 0);
	m_MatchingPlot.m_Color[2] = glm::vec3(0, 0, 1);

	m_UniformlySampledPlot.m_Color.resize(1);
	m_UniformlySampledPlot.m_Color[0] = glm::vec3(1, 0, 0);

	m_UniformlySampledPlot.m_yData.resize(1);

	m_RandomlySampledPlot.m_Color.resize(1);
	m_RandomlySampledPlot.m_Color[0] = glm::vec3(1, 0, 0);

	m_RandomlySampledPlot.m_yData.resize(1);

	m_MultipliedReflLumi.m_Color.resize(1);
	m_MultipliedReflLumi.m_Color[0] = glm::vec3(1, 0, 0);

	m_MultipliedReflLumi.m_yData.resize(1);

	m_ReflectancePlot.m_Color[0] = glm::vec3(128.f/255.f, 128.f / 255.f, 0);
	m_ReflectancePlot.m_Color[1] = glm::vec3(0, 1, 0);
	m_ReflectancePlot.m_Color[2] = glm::vec3(1, 0, 0);
	m_ReflectancePlot.m_Color[3] = glm::vec3(1, 1, 0);
	m_ReflectancePlot.m_Color[4] = glm::vec3(0, 1, 1);
	m_ReflectancePlot.m_Color[5] = glm::vec3(0.5f, 0.5f, 0.5f);

	m_ACIE = SpectralRendering::C_DataParser::parseSpectrum("spectral/A.cie");
	m_D65CIE = SpectralRendering::C_DataParser::parseSpectrum("spectral/D65.cie");
	m_F11CIE = SpectralRendering::C_DataParser::parseSpectrum("spectral/F11.cie");
	m_MatchingFunction = SpectralRendering::C_DataParser::parseMatchingFunction("spectral/");

	m_Colors[static_cast<int>(E_RadianceColors::E2)] = SpectralRendering::C_DataParser::parseSpectrum("spectral/E2.cie");
	m_Colors[static_cast<int>(E_RadianceColors::F4)] = SpectralRendering::C_DataParser::parseSpectrum("spectral/F4.cie");
	m_Colors[static_cast<int>(E_RadianceColors::G4)] = SpectralRendering::C_DataParser::parseSpectrum("spectral/G4.cie");
	m_Colors[static_cast<int>(E_RadianceColors::H4)] = SpectralRendering::C_DataParser::parseSpectrum("spectral/H4.cie");
	m_Colors[static_cast<int>(E_RadianceColors::J4)] = SpectralRendering::C_DataParser::parseSpectrum("spectral/J4.cie");
	m_Colors[static_cast<int>(E_RadianceColors::A1)] = SpectralRendering::C_DataParser::parseSpectrum("spectral/A1.cie");

	m_LuminairePlot.m_yData.resize(3);
	m_ACIE.FillData(m_LuminairePlot, 0);
	m_D65CIE.FillData(m_LuminairePlot, 1);
	m_F11CIE.FillData(m_LuminairePlot, 2);
	m_MatchingFunction.FillData(m_MatchingPlot);

	m_ReflectancePlot.m_yData.resize(6);
	m_Colors[static_cast<int>(E_RadianceColors::E2)].FillData(m_ReflectancePlot, 0);
	m_Colors[static_cast<int>(E_RadianceColors::F4)].FillData(m_ReflectancePlot, 1);
	m_Colors[static_cast<int>(E_RadianceColors::G4)].FillData(m_ReflectancePlot, 2);
	m_Colors[static_cast<int>(E_RadianceColors::H4)].FillData(m_ReflectancePlot, 3);
	m_Colors[static_cast<int>(E_RadianceColors::J4)].FillData(m_ReflectancePlot, 4);
	m_Colors[static_cast<int>(E_RadianceColors::A1)].FillData(m_ReflectancePlot, 5);



	m_XYZsRGB = glm::mat3(3.2410, -1.5374, -0.4986, -0.9692, 1.8760, 0.0416, 0.0556, -0.2040, 1.0570);


	{
		const auto sampled = m_F11CIE.SampleUniformly(m_Samples.GetValue());
		sampled.FillData(m_UniformlySampledPlot, 0);

		const auto multiplied = sampled * m_Colors[1].GetSameSamplig(sampled);

		multiplied.FillData(m_MultipliedReflLumi, 0);
	}
	{
		const auto sampled = m_F11CIE.SampleRandomly(m_Samples.GetValue());
		sampled.FillData(m_RandomlySampledPlot, 0);
	}
}

//=================================================================================
C_SpectralData::~C_SpectralData()
{
	static_cast<C_OGLRenderer*>(m_renderer.get())->DestroyControls(m_ImGUI->GetGUIMgr());
}

//=================================================================================
void C_SpectralData::Update()
{
	m_ImGUI->FrameBegin();
	m_ImGUI->OnUpdate();


	{
		using namespace Commands;
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth)
				)
			);
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLViewport>(0, 0, GetWidth(), GetHeight())
				)
			);
	}

	std::vector<float> x_data;
	std::vector<float> y_data;
	x_data.reserve(m_ACIE.GetNumSamples());
	y_data.reserve(m_ACIE.GetNumSamples());


	static ImU32 colors[3] = { ImColor(0, 255, 0), ImColor(255, 0, 0), ImColor(0, 0, 255) };
	static uint32_t selection_start = 0, selection_length = 0;
	
	::ImGui::Begin("Used plots", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	m_LuminairePlot.Draw();
	m_MatchingPlot.Draw();
	m_ReflectancePlot.Draw();
	::ImGui::End();

	::ImGui::Begin("Sampled plots", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		m_Samples.Draw();
		m_UniformlySampledPlot.Draw();
		m_RandomlySampledPlot.Draw();
	::ImGui::End();



	::ImGui::Begin("Multiplied", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		m_MultipliedReflLumi.Draw();
	::ImGui::End();
	
	if (m_Samples.Changed())
	{
		{
			const auto sampled = m_F11CIE.SampleUniformly(m_Samples.GetValue());
			sampled.FillData(m_UniformlySampledPlot, 0);

			const auto multiplied = sampled * m_Colors[1].GetSameSamplig(sampled);

			multiplied.FillData(m_MultipliedReflLumi, 0);
		}
		{
			const auto sampled = m_F11CIE.SampleRandomly(m_Samples.GetValue());
			sampled.FillData(m_RandomlySampledPlot, 0);
		}
	}

	// ----- Actual rendering --

	{
		RenderDoc::C_DebugScope s("RendererCommit");
		m_renderer->Commit();
	}
	m_renderer->ClearCommandBuffers();
	// ----- Actual rendering --

	{
		RenderDoc::C_DebugScope s("ImGUI");
		m_ImGUI->FrameEnd();
	}

	glfwSwapBuffers(m_Window);
}

//=================================================================================
void C_SpectralData::OnEvent(Core::I_Event& event)
{
	m_LayerStack.OnEvent(event);
}

//=================================================================================
bool C_SpectralData::OnAppInit(Core::C_AppEvent& event)
{
	return true;
}

}