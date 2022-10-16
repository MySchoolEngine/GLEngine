#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/FBO/Framebuffer.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/RenderInterface.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

namespace GLEngine::GLRenderer {

//=================================================================================
C_RenderInterface::C_RenderInterface(Shaders::C_ShaderManager& shmgr, Textures::C_TextureUnitManger& tm, C_OGLRenderer& renderer)
	: m_shmgr(shmgr)
	, m_tm(tm)
	, m_renderer(renderer)
{
	{
		// billboard
		Renderer::MeshData::Mesh billboardMesh;
		billboardMesh.vertices.emplace_back(-1.f, 1.f, 0, 1);  // 1
		billboardMesh.vertices.emplace_back(-1.f, -1.f, 0, 1); // 2
		billboardMesh.vertices.emplace_back(1.0f, 1.0f, 0, 1); // 3
		billboardMesh.vertices.emplace_back(-1.f, -1.f, 0, 1); // 4 = 2
		billboardMesh.vertices.emplace_back(1.f, -1.f, 0, 1);  // 5
		billboardMesh.vertices.emplace_back(1.0f, 1.0f, 0, 1); // 6 = 3


		billboardMesh.texcoords.emplace_back(0, 1);
		billboardMesh.texcoords.emplace_back(0, 0);
		billboardMesh.texcoords.emplace_back(1, 1);
		billboardMesh.texcoords.emplace_back(0, 0);
		billboardMesh.texcoords.emplace_back(1, 0);
		billboardMesh.texcoords.emplace_back(1, 1);

		m_ScreenQuad = std::make_shared<Mesh::C_StaticMeshResource>(billboardMesh);
	}
}

//=================================================================================
C_RenderInterface::~C_RenderInterface() = default;

//=================================================================================
bool C_RenderInterface::RenderFullScreen(const FullScreenSetup& setup)
{
	setup.renderTarget.Bind<E_FramebufferTarget::Draw>();
	auto shader = m_shmgr.GetProgram(setup.shaderName);
	if (!shader)
		return false;

	m_shmgr.ActivateShader(shader);

	int i = 0;
	for (const auto& texture : setup.inputTextures)
	{
		// needs cast
		m_tm.BindTextureToUnit(*static_cast<Textures::C_Texture*>(texture), i++);
	}
	m_renderer.AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>([&setup, shader]() { setup.shaderSetup(*(shader.get())); }, "Update full-screen pass shader"));

	m_renderer.AddCommand(std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_ScreenQuad));

	m_shmgr.DeactivateShader();
	setup.renderTarget.Unbind<E_FramebufferTarget::Draw>();
	return true;
}

} // namespace GLEngine::GLRenderer