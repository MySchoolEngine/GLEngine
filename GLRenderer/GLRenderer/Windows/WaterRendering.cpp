#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/GLRendererInterface2D.h>
#include <GLRenderer/OGLDevice.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/RenderInterface.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Textures/TextureUnitManager.h>
#include <GLRenderer/Windows/WaterRendering.h>

#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/Render/CPURasterizer.h>
#include <Renderer/Render/PhysicsRenderer.h>

#include <Editor/Editors/Image/Tools/BrickGenerator.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/2D/Plane2D.h>
#include <Physics/Constants.h>

#include <Core/Application.h>
#include <Core/Math/Rect.h>

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
		.property("SmoothingDistance", &C_WaterRendering::m_DensityRadius)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Smoothing distance:"),
			RegisterMetamember<UI::Slider::Min>(1.f),
			RegisterMetamember<UI::Slider::Max>(100.f))
		.property("PressureMultiplier", &C_WaterRendering::m_PressureMultiplier)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Pressure multiplier:"),
			RegisterMetamember<UI::Slider::Min>(1.f),
			RegisterMetamember<UI::Slider::Max>(500.f))
		.property("DensityDivisor", &C_WaterRendering::m_DensityDivisor)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Density divisor:"),
			RegisterMetamember<UI::Slider::Min>(1.f),
			RegisterMetamember<UI::Slider::Max>(100.f))
		.property("ParticleMass", &C_WaterRendering::m_ParticleMass)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Particle mass:"),
			RegisterMetamember<UI::Slider::Min>(1.f),
			RegisterMetamember<UI::Slider::Max>(100.f))
		.property("ParticleRadius", &C_WaterRendering::m_ParticleRadius)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Particle radius:"),
			RegisterMetamember<UI::Slider::Min>(1.f),
			RegisterMetamember<UI::Slider::Max>(100.f))
		.property("DesiredPressure", &C_WaterRendering::m_DesiredPressure)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Desired pressure:"),
			RegisterMetamember<UI::Slider::Min>(1.f),
			RegisterMetamember<UI::Slider::Max>(500.f))
		.property("RunSimulation", &C_WaterRendering::m_bRunSimulation)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Checkbox>(),
			RegisterMetamember<UI::Checkbox::Name>("Run simulation"))
		.property("ApplyGravitation", &C_WaterRendering::m_bGravitation)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Checkbox>(),
			RegisterMetamember<UI::Checkbox::Name>("Apply gravitation"));
}
// clang-format on

namespace GLEngine::GLRenderer {

static constexpr bool		s_SimulateOnGPU = false;
static constexpr glm::uvec2 s_Dimensions{800, 600};
static constexpr bool		s_indexed = false;
static const std::array		s_Planes  = {
	 Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{0.5f, 0.5f}), .Position = {200, 200}},
	 Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{-0.5f, 0.5f}), .Position = {s_Dimensions.x - 200, 200}},
	 // Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{0, 1.f}), .Position = {0, 200}}
};
static constexpr Core::S_Rect s_CollisionRect{20, 20, s_Dimensions.x - 40, s_Dimensions.y - 40};

//=================================================================================
C_WaterRendering::C_WaterRendering(const GUID guid, GUI::C_GUIManager& guiMGR, C_GLDevice& device)
	: C_Window(guid, "Water rendering")
	, m_Image({})
	, m_DrawCmd()
	, m_OverlayStorage(s_Dimensions.x, s_Dimensions.y, 4)
	, m_NumParticles(100)
	, m_DensityRadius(100.f)
	, m_PressureMultiplier(500.f)
	, m_DensityDivisor(6.25f)
	, m_ParticleRadius(5.f)
	, m_DesiredPressure(100.f)
	, m_ParticleMass(1.f)
	, m_bRunSimulation(false)
	, m_bGravitation(false)
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

	// overlay
	{
		using namespace Renderer;
		const TextureDescriptor waterOverlayDef{.name		   = "WaterRenderingOverlay",
												.width		   = s_Dimensions.x,
												.height		   = s_Dimensions.y,
												.type		   = E_TextureType::TEXTURE_2D,
												.format		   = E_TextureFormat::RGBA32f,
												.m_bStreamable = false};

		m_WorldOverlay = rrm.createTexture(waterOverlayDef);
		// renderer.SetTextureSampler(m_WorldOverlay, gpuSamplerHandle);


		C_TextureView view(&m_OverlayStorage);
		view.EnableBlending(true);

		// for (const auto& plane : s_Planes)
		// {
		// 	C_PhysicsRenderer::Render(view, plane, Colours::white);
		// }
		C_PhysicsRenderer::Render(view, s_CollisionRect, Colours::white);
		// C_CPURasterizer rasterizer(view);
		// rasterizer.DrawLine(Colours::white, glm::ivec2{400, 0}, glm::ivec2{0, 400}, true);
		// rasterizer.DrawLine(Colours::white, glm::ivec2{400, 0}, glm::ivec2{s_Dimensions.x-1, 400}, true);
		// rasterizer.DrawLine(Colours::white, glm::ivec2{0, 400}, glm::ivec2{ s_Dimensions.x-1, 400}, true);
		renderer.SetTextureData(m_WorldOverlay, m_OverlayStorage);
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
	m_Image.AddOverlay(m_WorldOverlay, true);
	m_Image.ShowLayers(true);

	m_RenderInterface
		= std::make_unique<C_RenderInterface>(Shaders::C_ShaderManager::Instance(), Textures::C_TextureUnitManger::Instance(), dynamic_cast<C_OGLRenderer&>(renderer));
}

//=================================================================================
void C_WaterRendering::DrawComponents() const
{
	m_Image.Draw();
	rttr::instance obj(*this);
	auto		   changed = GUI::DrawAllPropertyGUI(obj);
	if (changed.empty() == false)
	{
		if (::Utils::contains(changed, rttr::type::get<C_WaterRendering>().get_property("NumParticles"))
			|| ::Utils::contains(changed, rttr::type::get<C_WaterRendering>().get_property("ParticleRadius")))
		{
			m_bScheduledSetup = true;
		}
		if (::Utils::contains(changed, rttr::type::get<C_WaterRendering>().get_property("RunSimulation")))
		{
			m_Timer.reset();
		}
	}
}

//=================================================================================
void C_WaterRendering::Simulate()
{
	const float t = static_cast<float>(m_Timer.getElapsedTimeFromLastQueryMilliseconds()) / 1000.f;

	if (t > 0.5f)
	{
		return; // ignore long frames
	}

	if constexpr (s_SimulateOnGPU)
	{
		Shaders::C_ShaderManager& shm	  = Shaders::C_ShaderManager::Instance();
		auto					  program = shm.GetProgram("waterSimulation");
		if (program)
		{
			shm.ActivateShader(program);
			auto& renderer = dynamic_cast<C_OGLRenderer&>(Core::C_Application::Get().GetActiveRenderer());
			renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([&, program, t]() {
				auto& renderer = dynamic_cast<C_OGLRenderer&>(Core::C_Application::Get().GetActiveRenderer());
				auto& glRM	   = renderer.GetRMGR();
				program->SetUniform("deltaTime", t);
				program->SetUniform("numParticles", m_NumParticles);
				program->SetUniform("particleRadius", m_ParticleRadius);
				program->SetUniform("DensityRadius", m_DensityRadius);
				program->SetUniform("PressureMultiplier", m_PressureMultiplier);

				auto*	   buffer			= glRM.GetBuffer(m_ParticlesHandle);
				const auto uboBlockLocation = program->FindUniformBlockLocation("particlesUBO");
				buffer->BindBase(5);

				glDispatchCompute(m_NumParticles, 1, 1);
				glMemoryBarrier(GL_UNIFORM_BARRIER_BIT);
			}));
		}
	}
	else
	{
		for (auto& particle : m_Particles)
		{
			if (m_bGravitation)
				particle.Velocity += glm::vec2{0.f, -Physics::Constants::g * t};

			particle.LocalDensity		  = GetLocalDensity(particle.Position);
			const glm::vec2 pressureForce = CalculatePressureForce(particle.Position);
			particle.Velocity += pressureForce * particle.LocalDensity;
			particle.Move(t);
		}

		for (auto& particle : m_Particles)
		{
			Collision(particle, t);
		}
	}

	// m_bRunSimulation = false;
}

//=================================================================================
void C_WaterRendering::Collision(Particle& particle, const float t)
{
	constexpr static float dampingFactor = .9f;

	const auto previousPos = particle.Position - t * particle.Velocity;
	// need to move position by -Normal * particle size
	// detect exact contact point
	// calculate resulting vector

	static const std::array Planes = {
		Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{0.f, 1.f}), .Position = {0, s_CollisionRect.Top()}},
		Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{0.f, -1.f}), .Position = {0, s_CollisionRect.Bottom()}},
		Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{1.f, 0.f}), .Position = {s_CollisionRect.Left(), 0}},
		Physics::Primitives::Plane2D{.Normal = glm::normalize(glm::vec2{-1.f, 0.f}), .Position = {s_CollisionRect.Right(), 0}},
	};

	bool collisionInLastIteration = true;
	while (collisionInLastIteration)
	{
		collisionInLastIteration = false;
		struct {
			float								Sc = std::numeric_limits<float>::max();
			float								Se;
			const Physics::Primitives::Plane2D* plane;
		} closest;
		for (const auto& plane : Planes)
		{
			// Real-time rendering 4th edition - 25.9 Collision Response
			const float Sc = plane.DistanceToLine(previousPos);
			const float Se = plane.DistanceToLine(particle.Position);
			if (Sc * Se <= 0 || Sc <= m_ParticleRadius || Se <= m_ParticleRadius)
			{
				if (Sc < closest.Sc)
				{
					closest					 = {.Sc = Sc, .Se = Se, .plane = &plane};
					collisionInLastIteration = true;
				}
			}
		}

		if (collisionInLastIteration)
		{
			const float tFreeMove = (closest.Sc - m_ParticleRadius) / (closest.Sc - closest.Se);
			particle.Position	  = previousPos;
			particle.Move(t * tFreeMove);
			particle.Velocity = glm::reflect(particle.Velocity, closest.plane->Normal) * dampingFactor;

			particle.Move(t * (1.f - tFreeMove));
		}
	}
}

//=================================================================================
float C_WaterRendering::GetLocalDensity(const glm::vec2& position) const
{
	float density = 0.f;
	for (int i = 0; i < m_NumParticles; ++i)
	{
		const float distance = glm::distance(m_Particles[i].Position, position);
		if (distance == 0.f)
			continue;
		density += m_ParticleMass * SmoothingKernel(m_DensityRadius, distance);
	}
	return density;
}

//=================================================================================
void C_WaterRendering::Setup()
{
	m_bRunSimulation = false;
	m_Particles.clear();
	m_Particles.reserve(m_NumParticles);

	constexpr float padding		 = 5.f;
	constexpr auto	center		 = s_Dimensions / 2u;
	const auto		completeSize = 10.f * m_ParticleRadius * 2.f + 9.f * glm::vec2{padding, padding};
	const auto		topLeft		 = glm::uvec2{glm::ivec2{center} + glm::ivec2{-completeSize.x, completeSize.y} / 2};
	const auto		ParticleSize = glm::vec2{padding + m_ParticleRadius * 2.f, padding + m_ParticleRadius * 2.f};

	for (int i = 0; i < m_NumParticles; ++i)
	{
		const int line	 = i / 10;
		const int column = i % 10;

		m_Particles.emplace_back(glm::vec2{topLeft} + glm::vec2{ParticleSize.x * column, ParticleSize.y * line});
	}

	for (int i = 0; i < m_NumParticles; ++i)
	{
		m_Particles[i].LocalDensity = GetLocalDensity(m_Particles[i].Position);
	}

	auto&					   renderer = Core::C_Application::Get().GetActiveRenderer();
	Renderer::ResourceManager& rrm		= renderer.GetRM();
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
			 .size	= indirectBufferSize,
			 .type	= Renderer::E_BufferType::DrawIndirect,
			 .usage = Renderer::E_ResourceUsage::Immutable,
		 });
		renderer.SetBufferData(m_IndirectHandle, indirectBufferSize, &m_DrawCmd);
	}

	{
		if (m_ParticlesHandle)
		{
			rrm.destroyBuffer(m_ParticlesHandle);
		}

		const auto particlesBuffer = static_cast<unsigned int>(sizeof(Particle) * m_NumParticles);
		m_ParticlesHandle		   = rrm.createBuffer(Renderer::BufferDescriptor{
			 .size	= particlesBuffer,
			 .type	= Renderer::E_BufferType::ShaderStorage,
			 .usage = Renderer::E_ResourceUsage::Dynamic,
		 });
		renderer.SetBufferData(m_ParticlesHandle, particlesBuffer, m_Particles.data());
	}
}

//=================================================================================
float C_WaterRendering::GetLocalDensity(const Particle& samplingParticle) const
{
	float density = 0.f;
	for (const auto& particle : m_Particles)
	{
		const float distance = glm::distance(particle.Position, samplingParticle.Position);
		density += m_ParticleMass * SmoothingKernel(m_DensityRadius, distance);
	}
	return density;
}

//=================================================================================
float C_WaterRendering::SmoothingKernel(const float radius, const float distance)
{
	const auto	volume = (glm::pi<float>() * std::pow(radius, 8.f)) / 4.f;
	const float val	   = std::max(0.f, radius * radius - distance * distance);
	return val * val * val / volume;
}

//=================================================================================
float C_WaterRendering::SmoothingKernelDerivative(const float radius, const float distance)
{
	if (distance >= radius)
		return 0;
	const float val	  = std::max(0.f, radius * radius - distance * distance);
	const float scale = -24.f / (glm::pi<float>() * pow(radius, 8.f));
	return -scale * distance * val * val; // minus to keep derivation positive
}

//=================================================================================
float C_WaterRendering::SharpSmoothingKernel(const float radius, const float distance)
{
	const float volume = (glm::pi<float>() * std::pow(radius, 4.f)) / 6.f;
	const float value  = std::max(0.f, radius - distance);
	return value * value / volume;
}

//=================================================================================
float C_WaterRendering::SharpSmoothingKernelDerivative(const float radius, const float distance)
{
	if (distance >= radius)
		return 0;

	const float scale = -12.f / (std::pow(radius, 4.f) * glm::pi<float>());
	return (distance - radius) * scale;
}

//=================================================================================
float C_WaterRendering::ConvertDensityToPressure(const float density) const
{
	const float delta = density - m_DesiredPressure;
	return delta * m_PressureMultiplier;
}

//=================================================================================
glm::vec2 C_WaterRendering::CalculatePressureForce(const glm::vec2& pos) const
{
	glm::vec2 pressureForce = glm::vec2(0, 0);
	for (int i = 0; i < m_NumParticles; ++i)
	{
		const float distance = glm::distance(m_Particles[i].Position, pos);
		if (distance == 0.0)
			continue;
		const glm::vec2 dir			 = (m_Particles[i].Position - pos) / distance;
		const float		slope		 = SharpSmoothingKernelDerivative(m_DensityRadius, distance);
		const float		localDensity = m_Particles[i].LocalDensity;
		if (localDensity < 1e-4f)
		{
			continue;
		}
		pressureForce += ConvertDensityToPressure(localDensity) * dir * slope * m_ParticleMass / localDensity;
	}
	return pressureForce;
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
	if constexpr (s_indexed)
	{
		for (const auto& particle : m_Particles)
		{
			const auto density = GetLocalDensity(particle) / m_DensityDivisor;
			m_2DRenderer.Draw(Renderer::RenderCall2D{
				.Position		= {particle.Position},
				.Size			= {m_ParticleRadius * 2.f, m_ParticleRadius * 2.f},
				.Rotation		= 0.f,
				.PipelineHandle = m_Pipeline,
				.Colour			= glm::mix(Colours::blue, Colours::red, density),
			});
		}
	}
	else
	{
		m_2DRenderer.Draw(Renderer::RenderCall2D{
			.Position		= {200, 200},
			.Size			= {m_ParticleRadius * 2.f, m_ParticleRadius * 2.f},
			.Rotation		= 0.f,
			.PipelineHandle = m_Pipeline,
			.Colour			= Colours::red,
			.IndirectBuff	= m_IndirectHandle,
			.HackedArray	= m_ParticlesHandle,
		});
		if (!s_SimulateOnGPU)
		{
			auto&	   renderer		   = Core::C_Application::Get().GetActiveRenderer();
			const auto particlesBuffer = static_cast<unsigned int>(sizeof(Particle) * m_NumParticles);
			renderer.SetBufferData(m_ParticlesHandle, particlesBuffer, m_Particles.data());
		}
	}

	auto& renderer = dynamic_cast<C_OGLRenderer&>(Core::C_Application::Get().GetActiveRenderer());
	m_FBO->Bind<E_FramebufferTarget::Draw>();
	renderer.AddCommand(std::make_unique<Commands::C_GLClear>(Commands::C_GLClear::E_ClearBits::Color | Commands::C_GLClear::E_ClearBits::Depth));
	renderer.AddCommand(std::make_unique<Commands::C_GLViewport>(Renderer::C_Viewport(0, 0, s_Dimensions)));

	const FullScreenSetup fsSetup{
		.shaderName = "fluidDensity",
		.shaderSetup =
			[&](Shaders::C_ShaderProgram& shader) {
				auto& glRM	 = renderer.GetRMGR();
				auto* buffer = glRM.GetBuffer(m_ParticlesHandle);
				buffer->BindBase(5);
				shader.SetUniform("numParticles", m_NumParticles);
				shader.SetUniform("DensityRadius", m_DensityRadius);
				shader.SetUniform("Dimensions", s_Dimensions);
				shader.SetUniform("PressureMultiplier", m_PressureMultiplier);
			},
		.inputTextures = {},
		.renderTarget  = *m_FBO,
	};
	m_RenderInterface->RenderFullScreen(fsSetup);
	m_FBO->Bind<E_FramebufferTarget::Draw>();
	m_2DRenderer.Commit(*m_2DRenderInterfaceHandles.get());
	m_2DRenderer.Clear();
	m_FBO->Unbind<E_FramebufferTarget::Draw>();
}
} // namespace GLEngine::GLRenderer