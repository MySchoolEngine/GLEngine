#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/GLRendererInterface2D.h>
#include <GLRenderer/OGLDevice.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/RenderInterface.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Textures/TextureUnitManager.h>
#include <GLRenderer/Windows/RopeSimulation.h>

#include <Renderer/Descriptors/TextureDescriptor.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/Constants.h>

#include <Core/Application.h>

#include <Utils/StdVectorUtils.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::GLRenderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_RopeSimulation>("C_RopeSimulation")
		.property("RunSimulation", &C_RopeSimulation::m_bRunSimulation)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Checkbox>(),
			RegisterMetamember<UI::Checkbox::Name>("Run simulation"))
	;
}
// clang-format on

namespace GLEngine::GLRenderer {
static constexpr glm::uvec2					s_Dimensions{800, 600};
static constexpr Physics::Scalars::distance s_Distance{30.f};

//=================================================================================
C_RopeSimulation::C_RopeSimulation(const GUID guid, GUI::C_GUIManager& guiMGR, C_GLDevice& device)
	: C_Window(guid, "Rope simulation")
	, m_Image({})
	, m_OverlayStorage(s_Dimensions.x, s_Dimensions.y, 4)
	, m_DrawCmd()
	, m_NumParticles(10)
	, m_bScheduledSetup(true)
	, m_bRunSimulation(false)
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

		const TextureDescriptor ropeRenderingDef{.name			= "RopeSimulation",
												 .width			= s_Dimensions.x,
												 .height		= s_Dimensions.y,
												 .type			= E_TextureType::TEXTURE_2D,
												 .format		= E_TextureFormat::RGBA16f,
												 .m_bStreamable = false};
		m_DeviceImage = rrm.createTexture(ropeRenderingDef);
		renderer.SetTextureSampler(m_DeviceImage, gpuSamplerHandle);

		const TextureDescriptor depthTextureDef{.name		   = "RopeSimulationDepth",
												.width		   = s_Dimensions.x,
												.height		   = s_Dimensions.y,
												.type		   = E_TextureType::TEXTURE_2D,
												.format		   = E_TextureFormat::D16,
												.m_bStreamable = false};
		m_DeviceDepthImage = rrm.createTexture(depthTextureDef);
		renderer.SetTextureSampler(m_DeviceDepthImage, gpuSamplerHandle);
	}

	// overlay
	{
		using namespace Renderer;
		const TextureDescriptor ropeOverlayDef{.name		  = "RopeSimulationOverlay",
											   .width		  = s_Dimensions.x,
											   .height		  = s_Dimensions.y,
											   .type		  = E_TextureType::TEXTURE_2D,
											   .format		  = E_TextureFormat::RGBA32f,
											   .m_bStreamable = false};

		m_WorldOverlay = rrm.createTexture(ropeOverlayDef);
		renderer.SetTextureData(m_WorldOverlay, m_OverlayStorage);
	}

	// FBO
	{
		m_FBO = std::unique_ptr<C_Framebuffer>(device.AllocateFramebuffer("RopeSimulationFBO"));
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
																								.blendAlphaFunction = E_BlendFunction::Add,
																								.srcAlphaFactor		= E_BlendFactor::One,
																								.dstAlphaFactor		= E_BlendFactor::Zero}},
														   .bindingCount  = 1,
														   .vertexInput	  = {{.binding = 0, .type = T_TypeShaderDataType_v<glm::vec3>}},
														   .shader		  = "2D"});
	}

	m_Image = GUI::C_ImageViewer(m_DeviceImage);
	m_Image.SetSize(s_Dimensions);
	m_Image.AddOverlay(m_WorldOverlay, true);
	m_Image.ShowLayers(true);

	m_2DRenderInterfaceHandles = std::make_unique<C_GLRendererInterface2D>(s_Dimensions);

	m_RenderInterface
		= std::make_unique<C_RenderInterface>(Shaders::C_ShaderManager::Instance(), Textures::C_TextureUnitManger::Instance(), dynamic_cast<C_OGLRenderer&>(renderer));
}

//=================================================================================
void C_RopeSimulation::DrawComponents() const
{
	m_Image.Draw();
	rttr::instance obj(*this);
	auto		   changed = GUI::DrawAllPropertyGUI(obj);
	if (changed.empty() == false)
	{
		if (::Utils::contains(changed, rttr::type::get<C_RopeSimulation>().get_property("RunSimulation")))
		{
			m_Timer.reset();
		}
	}
}

//=================================================================================
void C_RopeSimulation::Setup()
{
	m_Particles.clear();
	m_Particles.reserve(m_NumParticles);

	for (int i = 0; i < m_NumParticles; ++i)
	{
		const auto initialPosition = Physics::Phys2D::position{glm::vec2{s_Dimensions.x / 2.f + i * s_Distance.get(), s_Dimensions.y - 50.f}};
		m_Particles.emplace_back(Particle{
			.Position	  = initialPosition,
			.PrevPosition = initialPosition,
			.Acceleration = i != 0 ? Physics::Constants::Phys2D::g : Physics::Phys2D::acceleration{0.f, 0.f},
			.fixed		  = i == 0 ? 1 : 0,
		});
	}

	m_Particles[9].Position = Physics::Phys2D::position{glm::vec2{s_Dimensions.x / 2.f + 8 * s_Distance.get(), s_Dimensions.y - 50.f}};
	m_Particles[9].fixed	= 1;

	auto&					   renderer = Core::C_Application::Get().GetActiveRenderer();
	Renderer::ResourceManager& rrm		= renderer.GetRM();

	// indirect draw command - this window only ever draws through the indirect path
	{
		if (m_IndirectHandle)
		{
			rrm.destroyBuffer(m_IndirectHandle);
		}

		m_DrawCmd = {
			.vertexCount   = 6,
			.instanceCount = static_cast<std::uint32_t>(m_NumParticles),
			.firstVertex   = 0,
			.firstInstance = 0,
		};

		constexpr auto indirectBufferSize = static_cast<unsigned int>(sizeof(Renderer::IndirectDraw));
		m_IndirectHandle				  = rrm.createBuffer(Renderer::BufferDescriptor{
			.size  = indirectBufferSize,
			.type  = Renderer::E_BufferType::DrawIndirect,
			.usage = Renderer::E_ResourceUsage::Immutable,
		});
		renderer.SetBufferData(m_IndirectHandle, indirectBufferSize, &m_DrawCmd);
	}

	if (m_ParticlesHandle)
	{
		rrm.destroyBuffer(m_ParticlesHandle);
	}

	const auto particlesBufferSize = static_cast<unsigned int>(sizeof(Particle) * m_NumParticles);
	m_ParticlesHandle			   = rrm.createBuffer(Renderer::BufferDescriptor{
		.size  = particlesBufferSize,
		.type  = Renderer::E_BufferType::ShaderStorage,
		.usage = Renderer::E_ResourceUsage::Dynamic,
	});
	renderer.SetBufferData(m_ParticlesHandle, particlesBufferSize, m_Particles.data());
}

//=================================================================================
void C_RopeSimulation::Simulate()
{
	using Physics::Scalars::time_typeLiterals::operator""_s;
	const auto t = Physics::Scalars::time_type{static_cast<float>(m_Timer.getElapsedTimeFromLastQueryMilliseconds()) / 1000.f};

	if (t > 0.5_s)
	{
		return; // ignore long frames
	}

	// Verlet integration
	for (auto& particle : m_Particles)
	{
		if (particle.fixed == 1)
			continue;
		const auto posBackup  = particle.Position;
		particle.Position	  = 2.f * particle.Position - Physics::Phys2D::d_position{particle.PrevPosition.get()} + t * particle.Acceleration * t;
		particle.PrevPosition = posBackup;
	}

	// Constraint relaxation
	for (int n = 0; n < 10; ++n)
	{
		for (unsigned int i = 1; i < m_Particles.size(); ++i)
		{
			RelaxConstraint(m_Particles[i - 1], m_Particles[i], s_Distance);
		}
	}


	const auto particlesBufferSize = static_cast<unsigned int>(sizeof(Particle) * m_NumParticles);
	auto&	   renderer			   = Core::C_Application::Get().GetActiveRenderer();
	renderer.SetBufferData(m_ParticlesHandle, particlesBufferSize, m_Particles.data());
}

//=================================================================================
void C_RopeSimulation::RelaxConstraint(Particle& p1, Particle& p2, Physics::Scalars::distance desiredDistance)
{
	if (p1.fixed && p2.fixed)
		return;
	const auto direction = glm::normalize(p2.Position.get() - p1.Position.get());
	const auto delta_d	 = Physics::Phys2D::getDistance(p1.Position, p2.Position) - desiredDistance;
	if (p1.fixed + p2.fixed == 0)
	{
		p1.Position += Physics::Phys2D::d_position{(delta_d.get() * direction) / 2.f};
		p2.Position -= Physics::Phys2D::d_position{(delta_d.get() * direction) / 2.f};
	}
	if (p1.fixed)
	{
		p2.Position -= Physics::Phys2D::d_position{(delta_d.get() * direction)};
	}
	if (p2.fixed)
	{
		p1.Position += Physics::Phys2D::d_position{(delta_d.get() * direction)};
	}
}

//=================================================================================
void C_RopeSimulation::RequestDestroy()
{
	C_Window::RequestDestroy();
}

//=================================================================================
bool C_RopeSimulation::CanDestroy() const
{
	return C_Window::CanDestroy();
}

//=================================================================================
void C_RopeSimulation::Update()
{
	if (m_bScheduledSetup)
	{
		Setup();
		m_bScheduledSetup = false;
	}

	if (m_bRunSimulation)
		Simulate();

	auto& renderer = dynamic_cast<C_OGLRenderer&>(Core::C_Application::Get().GetActiveRenderer());

	constexpr float particleRadius = 5.f;
	m_2DRenderer.Draw(Renderer::RenderCall2D{
		.Position		= {0, 0},
		.Size			= {particleRadius * 2.f, particleRadius * 2.f},
		.Rotation		= 0.f,
		.PipelineHandle = m_Pipeline,
		.Colour			= Colours::red,
		.IndirectBuff	= m_IndirectHandle,
		.HackedArray	= m_ParticlesHandle,
	});

	m_FBO->Bind<E_FramebufferTarget::Draw>();
	renderer.AddCommand(std::make_unique<Commands::C_GLClear>(Commands::C_GLClear::E_ClearBits::Color | Commands::C_GLClear::E_ClearBits::Depth));
	renderer.AddCommand(std::make_unique<Commands::C_GLViewport>(Renderer::C_Viewport(0, 0, s_Dimensions)));

	// const FullScreenSetup fsSetup{
	// 	.shaderName	   = "ropeSimulation",
	// 	.shaderSetup   = [](Shaders::C_ShaderProgram&) {},
	// 	.inputTextures = {},
	// 	.renderTarget  = *m_FBO,
	// };
	// m_RenderInterface->RenderFullScreen(fsSetup);

	m_FBO->Bind<E_FramebufferTarget::Draw>();
	m_2DRenderer.Commit(*m_2DRenderInterfaceHandles.get());
	m_2DRenderer.Clear();
	m_FBO->Unbind<E_FramebufferTarget::Draw>();
}
} // namespace GLEngine::GLRenderer
