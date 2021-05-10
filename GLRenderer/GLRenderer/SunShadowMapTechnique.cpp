#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/SunShadowMapTechnique.h>
#include <GLRenderer/Textures/Texture.h>

/// temporal
#include <GLRenderer/Commands/GLClear.h>
#include <GLRenderer/Commands/GLCullFace.h>
#include <GLRenderer/Commands/GLViewport.h>
#include <GLRenderer/Commands/GlClearColor.h>
/// ~temporal

#include <Renderer/ICameraComponent.h>
#include <Renderer/IRenderableComponent.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Lights/SunLight.h>

#include <Physics/Primitives/Frustum.h>

#include <Entity/EntityManager.h>
#include <Entity/IComponent.h>

#include <Core/Application.h>

#include <glm/gtc/matrix_transform.hpp>

namespace GLEngine::GLRenderer {
const std::uint32_t C_SunShadowMapTechnique::s_ShadowMapSize = 1024 * 2;
const float			C_SunShadowMapTechnique::s_ZOffset		 = 0.001f;


//=================================================================================
C_SunShadowMapTechnique::C_SunShadowMapTechnique(std::shared_ptr<Renderer::C_SunLight>& light)
	: m_Sun(light)
{
	m_FrameConstUBO = std::dynamic_pointer_cast<Buffers::UBO::C_FrameConstantsBuffer>(Buffers::C_UniformBuffersManager::Instance().GetBufferByName("frameConst"));


	m_Framebuffer = std::make_unique<C_Framebuffer>("sunShadowMapping");

	const bool storeAlbedoForShadowMap = true;
	if (storeAlbedoForShadowMap)
	{
		auto HDRTexture = std::make_shared<Textures::C_Texture>("hdrTexture");

		HDRTexture->bind();
		// HDRTexture setup
		HDRTexture->SetDimensions({s_ShadowMapSize, s_ShadowMapSize});
		HDRTexture->SetInternalFormat(Renderer::E_TextureFormat::RGBA8i, GL_RGBA);
		HDRTexture->SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
		// ~HDRTexture setup
		m_Framebuffer->AttachTexture(GL_COLOR_ATTACHMENT0, HDRTexture);
		HDRTexture->CreateHandle();
		HDRTexture->MakeHandleResident(true);
		HDRTexture->unbind();
	}
	else
	{
		auto& renderer = (Core::C_Application::Get()).GetActiveRenderer();
		m_Framebuffer->Bind();
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([=]() {
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}));
		m_Framebuffer->Bind();
	}

	auto depthTexture = std::make_shared<Textures::C_Texture>("sunShadowMap");

	depthTexture->bind();
	// depthStencilTexture setup
	depthTexture->SetDimensions({s_ShadowMapSize, s_ShadowMapSize});
	depthTexture->SetInternalFormat(Renderer::E_TextureFormat::D16, GL_DEPTH_COMPONENT);
	depthTexture->SetFilter(Renderer::E_TextureFilter::Nearest, Renderer::E_TextureFilter::Nearest);
	// depthTexture->SetTexParameter(GL_TEXTURE_BORDER_COLOR, glm::vec4(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
	// std::numeric_limits<float>::max()));
	depthTexture->SetWrap(Renderer::E_WrapFunction::ClampToBorder, Renderer::E_WrapFunction::ClampToBorder);

	m_Framebuffer->AttachTexture(GL_DEPTH_ATTACHMENT, depthTexture);

	depthTexture->CreateHandle();
	depthTexture->MakeHandleResident(true);

	depthTexture->unbind();

	// auto completeness = m_Framebuffer->CheckCompleteness();
	// const auto lambda = [](std::future<bool>&& completeness) {
	// 	if (!completeness.get())
	// 	{
	// 		CORE_LOG(E_Level::Error, E_Context::Render, "Shadow map fbo is uncomplete");
	// 	}
	// };
	//
	// m_Framebuffer->SetChecked();

	// std::thread completenessCheck(lambda, std::move(completeness));
	// completenessCheck.detach();
}

//=================================================================================
C_SunShadowMapTechnique::~C_SunShadowMapTechnique() = default;

//=================================================================================
void C_SunShadowMapTechnique::Render(const Entity::C_EntityManager& world, Renderer::I_CameraComponent* camera)
{
	Physics::Primitives::S_AABB aabb;
	for (const auto& entity : world.GetEntities())
	{
		aabb.Add(entity->GetAABB());
	}

	C_DebugDraw::Instance().DrawAABB(aabb, glm::vec3(1, 1, 1));

	const auto sun		= m_Sun.lock();
	const auto left		= glm::vec3(1, 0, 0);
	auto	   sunToTop = glm::normalize(glm::cross(left, sun->GetSunDirection()));
	if (sunToTop == glm::vec3(0.f))
	{
		sunToTop = glm::vec3(0, 1, 0);
	}

	const auto sunCenter	= glm::vec3(0.f);
	const auto sunDirection = glm::normalize(sun->GetSunDirection());

	const auto toLightSpace = glm::lookAt(sunDirection, sunCenter, sunToTop);

	const auto	transAABB = aabb.getTransformedAABB(toLightSpace);
	const float height	  = transAABB.m_Max.y - transAABB.m_Min.y;
	const float width	  = transAABB.m_Max.x - transAABB.m_Min.x;
	const auto	depth	  = transAABB.m_Max.z - transAABB.m_Min.z;

	auto middleOfFace = transAABB.m_Min + ((transAABB.m_Max - transAABB.m_Min) * 0.5f);
	middleOfFace.z	  = transAABB.m_Max.z;

	const auto sunCamPos = glm::inverse(toLightSpace) * glm::vec4(middleOfFace, 1.f);

	const Physics::Primitives::C_Frustum lightFrust(glm::vec3(sunCamPos) / sunCamPos.w + sunDirection * s_ZOffset, sunToTop, -sunDirection, s_ZOffset, depth + s_ZOffset,
													width / height, 0);

	const auto lightView = lightFrust.GetViewMatrix();

	const auto lightProjectionMatrix = glm::ortho(-width / 2.0f,  // left
												  width / 2.0f,	  // right
												  -height / 2.0f, // bottom
												  height / 2.0f,  // top
												  lightFrust.GetNear(), lightFrust.GetFar());

	m_LastViewProject = lightProjectionMatrix * lightView;

	auto& renderer = (Core::C_Application::Get()).GetActiveRenderer();
	renderer.SetCurrentPassType(Renderer::E_PassType::ShadowPass);

	m_Framebuffer->Bind<E_FramebufferTarget::Draw>();

	{
		using namespace Commands;
		renderer.AddCommand(std::make_unique<C_GLClear>(C_GLClear::E_ClearBits::Color | C_GLClear::E_ClearBits::Depth));
		renderer.AddCommand(std::make_unique<C_GLViewport>(Renderer::C_Viewport(0, 0, GetZBuffer()->GetDimensions())));
		renderer.AddCommand(std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Back));
	}

	{
		RenderDoc::C_DebugScope s("UBO Upload");
		renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
			[=]() {
				m_FrameConstUBO->SetView(lightView);
				m_FrameConstUBO->SetProjection(lightProjectionMatrix);
				m_FrameConstUBO->SetCameraPosition(glm::vec4(lightFrust.GetPosition(), 1.0f));
				m_FrameConstUBO->SetNearPlane(lightFrust.GetNear());
				m_FrameConstUBO->SetFarPlane(lightFrust.GetFar());
				m_FrameConstUBO->UploadData();
				m_FrameConstUBO->Activate(true);
			},
			"MainPass - upload UBOs"));
	}
	{
		const auto				entitiesInView = world.GetEntities(lightFrust);
		RenderDoc::C_DebugScope s("Commit geometry");
		for (auto& entity : entitiesInView)
		{
			auto renderableComponentsRange = entity->GetComponents(Entity::E_ComponentType::Graphical);
			for (const auto& it : renderableComponentsRange)
			{
				const auto rendarebleComp = component_cast<Entity::E_ComponentType::Graphical>(it);
				const auto compSphere	  = rendarebleComp->GetAABB().GetSphere();
				component_cast<Entity::E_ComponentType::Graphical>(it)->PerformDraw();
			}
		}
	}

	m_Framebuffer->Unbind<E_FramebufferTarget::Draw>();
	renderer.SetCurrentPassType(Renderer::E_PassType::FinalPass);

	GetZBuffer()->GenerateMipMaps();

	{
		using namespace Commands;
		renderer.AddCommand(std::make_unique<C_GLCullFace>(C_GLCullFace::E_FaceMode::Front));
	}
}

//=================================================================================
std::shared_ptr<GLEngine::GLRenderer::Textures::C_Texture> C_SunShadowMapTechnique::GetAlbedoTexture() const
{
	return m_Framebuffer->GetAttachement(GL_COLOR_ATTACHMENT0);
}

//=================================================================================
std::shared_ptr<GLEngine::GLRenderer::Textures::C_Texture> C_SunShadowMapTechnique::GetZBuffer() const
{
	return m_Framebuffer->GetAttachement(GL_DEPTH_ATTACHMENT);
}

//=================================================================================
glm::mat4 C_SunShadowMapTechnique::GetLastViewProjection() const
{
	return m_LastViewProject;
}

} // namespace GLEngine::GLRenderer