#include <GLRendererStdafx.h>

#include <GLRenderer/Windows/SpectralData.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/GlClearColor.h>

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
	, m_MultipliedReflLumiXYZ("MultipliedXYZ")
	, m_Results({{
			{"E2 A=({:.3f}, {:.3f}, {:.3f}) D65=({:.3f}, {:.3f}, {:.3f}) F11=({:.3f}, {:.3f}, {:.3f})"},
			{"F4 A=({:.3f}, {:.3f}, {:.3f}) D65=({:.3f}, {:.3f}, {:.3f}) F11=({:.3f}, {:.3f}, {:.3f})"},
			{"G4 A=({:.3f}, {:.3f}, {:.3f}) D65=({:.3f}, {:.3f}, {:.3f}) F11=({:.3f}, {:.3f}, {:.3f})"},
			{"H4 A=({:.3f}, {:.3f}, {:.3f}) D65=({:.3f}, {:.3f}, {:.3f}) F11=({:.3f}, {:.3f}, {:.3f})"},
			{"J4 A=({:.3f}, {:.3f}, {:.3f}) D65=({:.3f}, {:.3f}, {:.3f}) F11=({:.3f}, {:.3f}, {:.3f})"},
			{"A1 A=({:.3f}, {:.3f}, {:.3f}) D65=({:.3f}, {:.3f}, {:.3f}) F11=({:.3f}, {:.3f}, {:.3f})"},
		}})
	, m_UpdateResults("Update results", [&]() {UpdateResults(); })
	, m_SamplingButtons({{
		{"Uniform sampling", [&]() { SelectSampling(E_Sampling::Uniform); UpdateSampledPlots(m_Samples.GetValue()); }},
		{"Random sampling", [&]() { SelectSampling(E_Sampling::Random); UpdateSampledPlots(m_Samples.GetValue()); }},
		{"Hero wavelength", [&]() { SelectSampling(E_Sampling::Hero); UpdateSampledPlots(m_Samples.GetValue()); }}
		}})
	, m_SelectedSampling("Selected sampling is: {}")
	, m_ColorsButtons({{
		{"E2", [&]() { SelectColor(E_RadianceColors::E2); }},
		{"F4", [&]() { SelectColor(E_RadianceColors::F4); }},
		{"G4", [&]() { SelectColor(E_RadianceColors::G4); }},
		{"H4", [&]() { SelectColor(E_RadianceColors::H4); }},
		{"J4", [&]() { SelectColor(E_RadianceColors::J4); }},
		{"A1", [&]() { SelectColor(E_RadianceColors::A1); }},
	}})
	, m_SelectedColorString("Selected color: {}")
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

	m_HeroSampledPlot.m_Color.resize(1);
	m_HeroSampledPlot.m_Color[0] = glm::vec3(1, 0, 0);

	m_HeroSampledPlot.m_yData.resize(1);

	m_MultipliedReflLumi.m_Color.resize(1);
	m_MultipliedReflLumi.m_Color[0] = glm::vec3(1, 0, 0);

	m_MultipliedReflLumi.m_yData.resize(1);

	m_MultipliedReflLumiXYZ.m_Color.resize(3);
	m_MultipliedReflLumiXYZ.m_Color[0] = glm::vec3(1, 0, 0);
	m_MultipliedReflLumiXYZ.m_Color[1] = glm::vec3(0, 1, 0);
	m_MultipliedReflLumiXYZ.m_Color[2] = glm::vec3(0, 0, 1);

	m_MultipliedReflLumiXYZ.m_yData.resize(3);

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

	SelectSampling(E_Sampling::Uniform);
	SelectColor(E_RadianceColors::E2);

	UpdateBackground();

	UpdateSampledPlots(m_Samples.GetValue());

	UpdateResults();
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
		m_renderer->AddCommand(
			std::move(
				std::make_unique<C_GLClearColor>(m_ClearColor)
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
	for (int i = 0; i < 6; ++i)
	{
		m_ColorsButtons[i].Draw();
	}
	m_SelectedColorString.Draw();
	m_ReflectancePlot.Draw();
	::ImGui::End();

	::ImGui::Begin("Sampled plots", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		m_Samples.Draw();
		m_SamplingButtons[0].Draw();
		m_SamplingButtons[1].Draw();
		m_SamplingButtons[2].Draw();
		m_SelectedSampling.Draw();
		m_UniformlySampledPlot.Draw();
		m_RandomlySampledPlot.Draw();
		m_HeroSampledPlot.Draw();
	::ImGui::End();


	::ImGui::Begin("Multiplied XYZ", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		m_MultipliedReflLumiXYZ.Draw();
	::ImGui::End();



	::ImGui::Begin("Multiplied", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		m_MultipliedReflLumi.Draw();
	::ImGui::End();


	::ImGui::Begin("Results", nullptr);
	m_UpdateResults.Draw();
	for (int i = 0; i < 6; ++i)
	{
		m_Results[i].Draw();
	}
	::ImGui::End();
	
	if (m_Samples.Changed())
	{
		UpdateBackground();
		UpdateSampledPlots(m_Samples.GetValue());
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

//=================================================================================
void C_SpectralData::UpdateResults()
{
	glm::vec3 a;
	glm::vec3 d65;
	glm::vec3 f11;

	for (int i = 0; i < 6; ++i)
	{
		{
			const auto sampled = m_CurrentSampling(m_ACIE);

			const auto multiplied = sampled * m_Colors[i].GetSameSamplig(sampled);

			glm::vec3 xyz = multiplied.GetXYZ(m_MatchingFunction);
			a = xyz * m_XYZsRGB;
		}
		{
			const auto sampled = m_CurrentSampling(m_D65CIE);

			const auto multiplied = sampled * m_Colors[i].GetSameSamplig(sampled);

			glm::vec3 xyz = multiplied.GetXYZ(m_MatchingFunction);
			d65 = xyz * m_XYZsRGB;
		}
		{
			const auto sampled = m_CurrentSampling(m_F11CIE);

			const auto multiplied = sampled * m_Colors[i].GetSameSamplig(sampled);

			glm::vec3 xyz = multiplied.GetXYZ(m_MatchingFunction);
			f11 = xyz * m_XYZsRGB;
		}
		
		m_Results[i].UpdateText(a.x, a.y, a.z, d65.x, d65.y, d65.z, f11.x, f11.y, f11.z);
	}
}

//=================================================================================
void C_SpectralData::SelectSampling(E_Sampling sampling)
{
	switch (sampling)
	{
	case E_Sampling::Uniform:
		m_CurrentSampling = [&](const SpectralRendering::C_Spectrum& sam) {return sam.SampleUniformly(m_Samples.GetValue());};
		m_SelectedSampling.UpdateText("Uniform");
		break;
	case E_Sampling::Random:
		m_CurrentSampling = [&](const SpectralRendering::C_Spectrum& sam) {return sam.SampleRandomly(m_Samples.GetValue()); };
		m_SelectedSampling.UpdateText("Random");
		break;
	case E_Sampling::Hero:
		m_CurrentSampling = [&](const SpectralRendering::C_Spectrum& sam) {return sam.SampleHero(m_Samples.GetValue()); };
		m_SelectedSampling.UpdateText("Hero");
		break;
	default:
		break;
	}
}

//=================================================================================
void C_SpectralData::SelectColor(E_RadianceColors color)
{
	m_SelectedColor = color;
	switch (color)
	{
	case C_SpectralData::E_RadianceColors::E2:
		m_SelectedColorString.UpdateText("E2");
		break;
	case C_SpectralData::E_RadianceColors::F4:
		m_SelectedColorString.UpdateText("F4");
		break;
	case C_SpectralData::E_RadianceColors::G4:
		m_SelectedColorString.UpdateText("G4");
		break;
	case C_SpectralData::E_RadianceColors::H4:
		m_SelectedColorString.UpdateText("H4");
		break;
	case C_SpectralData::E_RadianceColors::J4:
		m_SelectedColorString.UpdateText("J4");
		break;
	case C_SpectralData::E_RadianceColors::A1:
		m_SelectedColorString.UpdateText("A1");
		break;
	default:
		break;
	}
	UpdateBackground(); 
	UpdateSampledPlots(m_Samples.GetValue());
}

//=================================================================================
void C_SpectralData::UpdateBackground()
{
	const auto sampled = m_ACIE.SampleUniformly(m_Samples.GetValue());

	const auto multiplied = sampled * m_Colors[static_cast<int>(m_SelectedColor)].GetSameSamplig(sampled);

	glm::vec3 xyz = multiplied.GetXYZ(m_MatchingFunction);
	m_ClearColor = xyz * m_XYZsRGB;
}

//=================================================================================
void C_SpectralData::UpdateSampledPlots(const int samples)
{
	{
		const auto sampled = m_F11CIE.SampleUniformly(samples);

		sampled.FillData(m_UniformlySampledPlot, 0);
	}
	{
		const auto sampled = m_F11CIE.SampleRandomly(samples);

		sampled.FillData(m_RandomlySampledPlot, 0);
	}
	{
		const auto sampled = m_F11CIE.SampleHero(samples);

		sampled.FillData(m_HeroSampledPlot, 0);
	}


	{
		const auto sampled = m_CurrentSampling(m_F11CIE);

		const auto multiplied = sampled * m_Colors[static_cast<int>(m_SelectedColor)].GetSameSamplig(sampled);

		m_MultipliedReflLumi.m_Color[0] = m_ReflectancePlot.m_Color[static_cast<int>(m_SelectedColor)];

		multiplied.FillData(m_MultipliedReflLumi, 0);


		const auto X = multiplied * m_MatchingFunction.m_Samples[0].GetSameSamplig(multiplied);
		const auto Y = multiplied * m_MatchingFunction.m_Samples[1].GetSameSamplig(multiplied);
		const auto Z = multiplied * m_MatchingFunction.m_Samples[2].GetSameSamplig(multiplied);


		X.FillData(m_MultipliedReflLumiXYZ, 0);
		Y.FillData(m_MultipliedReflLumiXYZ, 1);
		Z.FillData(m_MultipliedReflLumiXYZ, 2);
	}
}

}