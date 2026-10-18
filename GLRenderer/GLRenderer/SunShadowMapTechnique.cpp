#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/OGLDevice.h>
#include <GLRenderer/SunShadowMapTechnique.h>
#include <GLRenderer/Textures/Texture.h>

/// temporal
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLCullFace.h>
#include <GLRenderer/Commands/GLViewport.h>
/// ~temporal

#include <Renderer/Components/Lights/LightComponents.h>
#include <Renderer/Components/Mesh/GeomComponents.h>
#include <Renderer/Components/Mesh/StaticMeshComponents.h>
#include <Renderer/ICamera.h>
#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Renderer3D.h>
#include <Renderer/Systems/GeomSystem.h>
#include <Renderer/Systems/StaticMeshSystem.h>

#include <Physics/Primitives/Frustum.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/World.h>

#include <Core/Application.h>

#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine::GLRenderer {
const std::uint32_t C_SunShadowMapTechnique::s_ShadowMapSize = 1024 * 2;
const float			C_SunShadowMapTechnique::s_ZOffset		 = 0.001f;


//=================================================================================
C_SunShadowMapTechnique::C_SunShadowMapTechnique(entt::entity sun)
	: m_Sun(sun)
{
	m_FrameConstUBO = std::dynamic_pointer_cast<Buffers::UBO::C_FrameConstantsBuffer>(Buffers::C_UniformBuffersManager::Instance().GetBufferByName("frameConst"));

	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& device   = dynamic_cast<C_GLDevice&>(renderer.GetDevice());
	auto& glRM	   = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();

	m_Framebuffer = std::unique_ptr<C_Framebuffer>(device.AllocateFramebuffer("sunShadowMapping"));


	constexpr bool storeAlbedoForShadowMap = true;
	if (storeAlbedoForShadowMap)
	{
		using namespace Renderer;
		const auto				gpuSamplerHandle = renderer.GetRM().createSampler(SamplerDescriptor2D{
			.m_FilterMin = E_TextureFilter::Linear,
			.m_FilterMag = E_TextureFilter::Linear,
			.m_WrapS	 = E_WrapFunction::Repeat,
			.m_WrapT	 = E_WrapFunction::Repeat,
			.m_WrapU	 = E_WrapFunction::Repeat,
		});
		const TextureDescriptor textureDescriptor{.name			 = "hdrTexture",
												  .width		 = s_ShadowMapSize,
												  .height		 = s_ShadowMapSize,
												  .type			 = E_TextureType::TEXTURE_2D,
												  .format		 = E_TextureFormat::RGBA16f,
												  .m_bStreamable = false};

		auto hdrTexture = renderer.GetRM().createTexture(textureDescriptor);
		renderer.SetTextureSampler(hdrTexture, gpuSamplerHandle);

		m_Framebuffer->AttachTexture(GL_COLOR_ATTACHMENT0, hdrTexture);
		auto* glTexture = glRM.GetTexture(hdrTexture);
		glTexture->SetReadyToUse();
		GLE_TODO("27-12-2024", "RohacekD", "This is nasty hack. Calling this here woudl cause getting error texture (Identity wasn't uploaded to GPU yet)");
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
			[glTexture]() {
				std::ignore = glTexture->CreateHandle(); // side effect: caches the bindless handle, retrieved later via GetHandle()
			},
			"Create identity handle"));
		glTexture->MakeHandleResident(true);
	}
	else
	{
		m_Framebuffer->Bind();
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([=]() {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}));
		m_Framebuffer->Bind();
	}

	{
		using namespace Renderer;
		const auto		  gpuSamplerHandle = renderer.GetRM().createSampler(SamplerDescriptor2D{
			.m_FilterMin = E_TextureFilter::Nearest,
			.m_FilterMag = E_TextureFilter::Nearest,
			.m_WrapS	 = E_WrapFunction::ClampToBorder,
			.m_WrapT	 = E_WrapFunction::ClampToBorder,
			.m_WrapU	 = E_WrapFunction::ClampToBorder,
		});
		TextureDescriptor shadowMapDesc{.name		   = "sunShadowMap",
										.width		   = s_ShadowMapSize,
										.height		   = s_ShadowMapSize,
										.type		   = E_TextureType::TEXTURE_2D,
										.format		   = E_TextureFormat::D16,
										.m_bStreamable = false};

		auto depthTexture = renderer.GetRM().createTexture(shadowMapDesc);
		renderer.SetTextureSampler(depthTexture, gpuSamplerHandle);
		auto* glDepthTexture = glRM.GetTexture(depthTexture);


		// depthTexture->SetTexParameter(GL_TEXTURE_BORDER_COLOR, glm::vec4(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
		// std::numeric_limits<float>::max()));

		m_Framebuffer->AttachTexture(GL_DEPTH_ATTACHMENT, depthTexture);
		glDepthTexture->SetReadyToUse();

		std::ignore = glDepthTexture->CreateHandle(); // side effect: caches the bindless handle, retrieved later via GetHandle()
		glDepthTexture->MakeHandleResident(true);
	}
}

//=================================================================================
C_SunShadowMapTechnique::~C_SunShadowMapTechnique() = default;

//=================================================================================
void C_SunShadowMapTechnique::Render(Entity::C_World& world, Renderer::I_Camera* camera, Renderer::I_RenderInterface3D& renderInterface3D)
{
	auto& registry = world.Registry();

	Physics::Primitives::S_AABB aabb;
	for (auto [e, bounds, worldTransform] : registry.view<Entity::S_LocalBounds, Entity::S_WorldTransformComponent>().each())
	{
		aabb.Add(bounds.aabb.getTransformedAABB(worldTransform.world));
	}

	C_DebugDraw::Instance().DrawAABB(aabb, glm::vec3(1, 1, 1));

	const auto&		sun = registry.get<Renderer::S_SunLight>(m_Sun);
	const glm::vec3 sunDirectionRaw(sun.sunX, sun.sunY, sun.sunZ);
	constexpr auto	left	 = glm::vec3(1, 0, 0);
	auto			sunToTop = glm::normalize(glm::cross(left, sunDirectionRaw));
	if (sunToTop == glm::vec3(0.f))
	{
		sunToTop = glm::vec3(0, 1, 0);
	}

	constexpr auto sunCenter	= glm::vec3(0.f);
	const auto	   sunDirection = glm::normalize(sunDirectionRaw);

	const auto toLightSpace = glm::lookAt(sunDirection, sunCenter, sunToTop);

	const auto	transAABB = aabb.getTransformedAABB(toLightSpace);
	const float height	  = transAABB.m_Max.y - transAABB.m_Min.y;
	const float width	  = transAABB.m_Max.x - transAABB.m_Min.x;
	const auto	depth	  = transAABB.m_Max.z - transAABB.m_Min.z;

	auto middleOfFace = transAABB.m_Min + ((transAABB.m_Max - transAABB.m_Min) * 0.5f);
	middleOfFace.z	  = transAABB.m_Max.z;

	const auto sunCamPos = glm::inverse(toLightSpace) * glm::vec4(middleOfFace, 1.f);

	const Physics::Primitives::C_Frustum lightFrustum(glm::vec3(sunCamPos) / sunCamPos.w + sunDirection * s_ZOffset, sunToTop, -sunDirection, s_ZOffset, depth + s_ZOffset,
													  width / height, 0);

	const auto lightView = lightFrustum.GetViewMatrix();

	const auto lightProjectionMatrix = glm::ortho(-width / 2.0f,  // left
												  width / 2.0f,	  // right
												  -height / 2.0f, // bottom
												  height / 2.0f,  // top
												  lightFrustum.GetNear(), lightFrustum.GetFar());

	m_LastViewProject = lightProjectionMatrix * lightView;

	auto& renderer = (Core::C_Application::Get()).GetActiveRenderer();
	renderer.SetCurrentPassType(Renderer::E_PassType::ShadowPass);

	m_Framebuffer->Bind<E_FramebufferTarget::Draw>();

	{
		using namespace Commands;
		renderer.AddCommand(std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth));
		renderer.AddCommand(std::make_unique<C_GLViewport>(Renderer::C_Viewport(0, 0, {s_ShadowMapSize, s_ShadowMapSize})));
		renderer.AddCommand(std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Back));
	}

	{
		RenderDoc::C_DebugScope s("UBO Upload");
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
			[&]() {
				m_FrameConstUBO->SetView(lightView);
				m_FrameConstUBO->SetProjection(lightProjectionMatrix);
				m_FrameConstUBO->SetCameraPosition(glm::vec4(lightFrustum.GetPosition(), 1.0f));
				m_FrameConstUBO->SetNearPlane(lightFrustum.GetNear());
				m_FrameConstUBO->SetFarPlane(lightFrustum.GetFar());
				m_FrameConstUBO->UploadData(renderer);
				m_FrameConstUBO->Activate(renderer.GetRM(), true);
			},
			"MainPass - upload UBOs"));
	}
	{
		// No frustum culling here, matching the old GetEntities(C_Frustum) overload this replaces
		// (confirmed a no-op - it unconditionally returned every entity regardless of frustum).
		RenderDoc::C_DebugScope s("Commit geometry");
		for (auto [e, mesh, gpu, worldTransform] : registry.view<Renderer::S_StaticMesh, Renderer::S_StaticMeshGPU, Entity::S_WorldTransformComponent>().each())
		{
			Renderer::StaticMeshSystem::Render(mesh, gpu, worldTransform, m_3DRenderer);
		}
		for (auto [e, geom, gpu, worldTransform] : registry.view<Renderer::S_GeomComponent, Renderer::S_GeomGPU, Entity::S_WorldTransformComponent>().each())
		{
			Renderer::GeomSystem::Render(geom, gpu, worldTransform, m_3DRenderer);
		}
	}

	{
		RenderDoc::C_DebugScope s("3D Renderer");
		m_3DRenderer.Commit(renderInterface3D);
		m_3DRenderer.Clear();
	}

	m_Framebuffer->Unbind<E_FramebufferTarget::Draw>();
	renderer.SetCurrentPassType(Renderer::E_PassType::FinalPass);

	auto& glRM = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR();
	glRM.GetTexture(GetZBuffer())->GenerateMipMaps();

	{
		using namespace Commands;
		renderer.AddCommand(std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Front));
	}
}

//=================================================================================
Renderer::Handle<Renderer::Texture> C_SunShadowMapTechnique::GetAlbedoTexture() const
{
	return m_Framebuffer->GetAttachment(GL_COLOR_ATTACHMENT0);
}

//=================================================================================
Renderer::Handle<Renderer::Texture> C_SunShadowMapTechnique::GetZBuffer() const
{
	return m_Framebuffer->GetAttachment(GL_DEPTH_ATTACHMENT);
}

//=================================================================================
glm::mat4 C_SunShadowMapTechnique::GetLastViewProjection() const
{
	return m_LastViewProject;
}

} // namespace GLEngine::GLRenderer