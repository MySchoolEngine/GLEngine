#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/GLRendererInterface2D.h>
#include <GLRenderer/OGLDevice.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/Windows/WaterRendering.h>

#include <Renderer/Descriptors/TextureDescriptor.h>

#include <Editor/Editors/Image/Tools/BrickGenerator.h>

#include <GUI/ReflectionGUI.h>

#include <Core/Application.h>

#include <Utils/StdVectorUtils.h>

#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::GLRenderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_WaterRendering>("C_WaterRendering")
		.property("NumParticles", &C_WaterRendering::m_NumParticles)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::SliderInt>(),
			RegisterMetamember<UI::SliderInt::Name>("Num particles:"),
			RegisterMetamember<UI::SliderInt::Min>(1),
			RegisterMetamember<UI::SliderInt::Max>(100))
		.property("NumParticles", &C_WaterRendering::m_NumParticles)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Checkbox>(),
			RegisterMetamember<UI::Checkbox::Name>("Run simulation"));
}
// clang-format on

namespace GLEngine::GLRenderer {


static constexpr glm::vec2	s_ParticleSize(30, 30);
static constexpr glm::uvec2 s_Dimensions{800, 600};

//=================================================================================
C_WaterRendering::C_WaterRendering(GUID guid, GUI::C_GUIManager& guiMGR, C_GLDevice& device)
	: C_Window(guid, "Water rendering")
	, m_Image({})
	, m_NumParticles(1)
	, m_bScheduledSetup(true)
{
	auto&					   renderer = Core::C_Application::Get().GetActiveRenderer();
	Renderer::ResourceManager& rrm		= renderer.GetRM();

	// Texture
	{
		using namespace Renderer;
		const auto gpuSamplerHandle = rrm.createSampler(SamplerDescriptor2D{
			.m_FilterMin = E_TextureFilter::Linear,
			.m_FilterMag = E_TextureFilter::Linear,
			.m_WrapS	 = E_WrapFunction::Repeat,
			.m_WrapT	 = E_WrapFunction::Repeat,
			.m_WrapU	 = E_WrapFunction::Repeat,
		});

		const TextureDescriptor waterRenderingDef{.name			 = "WaterRendering",
												  .width		 = s_Dimensions.x,
												  .height		 = s_Dimensions.y,
												  .type			 = E_TextureType::TEXTURE_2D,
												  .format		 = E_TextureFormat::RGBA16f,
												  .m_bStreamable = false};
		m_DeviceImage = rrm.createTexture(waterRenderingDef);
		renderer.SetTextureSampler(m_DeviceImage, gpuSamplerHandle);

		const TextureDescriptor hdrDepthTextureDef{.name		  = "WaterRenderingDepth",
												   .width		  = s_Dimensions.x,
												   .height		  = s_Dimensions.y,
												   .type		  = E_TextureType::TEXTURE_2D,
												   .format		  = E_TextureFormat::D16,
												   .m_bStreamable = false};
		m_DeviceDepthImage = rrm.createTexture(hdrDepthTextureDef);
		renderer.SetTextureSampler(m_DeviceDepthImage, gpuSamplerHandle);
	}

	// FBO
	{
		m_FBO = std::unique_ptr<C_Framebuffer>(device.AllocateFramebuffer("WaterRenderingFBO"));
		m_FBO->AttachTexture(GL_COLOR_ATTACHMENT0, m_DeviceImage);
		m_FBO->AttachTexture(GL_DEPTH_ATTACHMENT, m_DeviceDepthImage);
	}

	// pipeline
	{
		using namespace Renderer;
		m_Pipeline = rrm.createPipeline(PipelineDescriptor{.primitiveType = E_RenderPrimitives::TriangleList,
														   .blending	  = {BlendingDescriptor{.enable				= true,
																								.blendColorFunction = E_BlendFunction::Add,
																								.srcColorFactor		= E_BlendFactor::SourceAlpha,
																								.dstColorFactor		= E_BlendFactor::InvSourceAlpha,
																								.blendAlphaFunction = E_BlendFunction::Add, // todo
																								.srcAlphaFactor		= E_BlendFactor::One,
																								.dstAlphaFactor		= E_BlendFactor::Zero}},
														   .bindingCount  = 1,
														   .vertexInput	  = {{.binding = 0, .type = T_TypeShaderDataType_v<glm::vec3>}},
														   .shader		  = "2D"});
	}
	m_2DRenderInterfaceHandles = std::make_unique<C_GLRendererInterface2D>(s_Dimensions);


	m_Image = GUI::C_ImageViewer(m_DeviceImage);
	m_Image.SetSize(s_Dimensions);
}

//=================================================================================
void C_WaterRendering::DrawComponents() const
{
	m_Image.Draw();
	rttr::instance obj(*this);
	auto		   changed = GUI::DrawAllPropertyGUI(obj);
	if (changed.empty() == false)
	{
		if (::Utils::contains(changed, rttr::type::get<C_WaterRendering>().get_property("NumParticles")))
		{
			m_bScheduledSetup = true;
		}
	}
}

//=================================================================================
void C_WaterRendering::Simulate()
{
}

//=================================================================================
void C_WaterRendering::Setup()
{
	m_bRunSimulation = false;
	m_Particles.clear();
	m_Particles.reserve(m_NumParticles);

	constexpr float padding		 = 5.f;
	constexpr auto	center		 = s_Dimensions / 2u;
	constexpr auto	completeSize = 10.f * s_ParticleSize + 9.f * glm::vec2{padding, padding};
	constexpr auto	topLeft		 = center - glm::uvec2{completeSize} / 2u;
	constexpr auto	ParticleSize = padding + s_ParticleSize;

	for (int i = 0; i < m_NumParticles; ++i)
	{
		const int line	 = i / 10;
		const int column = i % 10;

		m_Particles.emplace_back(glm::vec2{topLeft} + glm::vec2{ParticleSize.x * column, ParticleSize.y * line});
	}
}

//=================================================================================
void C_WaterRendering::RequestDestroy()
{
	C_Window::RequestDestroy();
}

//=================================================================================
bool C_WaterRendering::CanDestroy() const
{
	return C_Window::CanDestroy();
}

//=================================================================================
void C_WaterRendering::Update()
{
	if (m_bScheduledSetup)
	{
		Setup();
		m_bScheduledSetup = false;
	}

	if (m_bRunSimulation)
		Simulate();

	RenderDoc::C_DebugScope s("Water draw");
	for (const auto& particle : m_Particles)
	{
		m_2DRenderer.Draw(Renderer::RenderCall2D{
			.Position		= particle,
			.Size			= s_ParticleSize,
			.Rotation		= 0.f,
			.PipelineHandle = m_Pipeline,
		});
	}
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	m_FBO->Bind<E_FramebufferTarget::Draw>();
	renderer.AddCommand(std::make_unique<Commands::C_GLClear>(Commands::C_GLClear::E_ClearBits::Color | Commands::C_GLClear::E_ClearBits::Depth));
	m_2DRenderer.Commit(*m_2DRenderInterfaceHandles.get());
	m_2DRenderer.Clear();
	m_FBO->Unbind<E_FramebufferTarget::Draw>();
}
} // namespace GLEngine::GLRenderer