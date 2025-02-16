#include <GLRendererStdafx.h>

#include <GLRenderer/CameraManager.h>
#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Components/SkyBox.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/TextureLoader.h>

#include <Core/Application.h>

#include <pugixml.hpp>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_SkyBox::C_SkyBox(const std::shared_ptr<Entity::I_Entity>& owner)
	: Renderer::I_RenderableComponent(owner)
//, m_Textures("Skybox", GL_TEXTURE_CUBE_MAP)
{
	//m_Textures.SetFilter(Renderer::E_TextureFilter::Linear, Renderer::E_TextureFilter::Linear);
	//m_Textures.SetWrap(Renderer::E_WrapFunction::ClampToEdge, Renderer::E_WrapFunction::ClampToEdge, Renderer::E_WrapFunction::ClampToEdge);

	std::vector<glm::vec3> vertices;
	// left
	vertices.emplace_back(100.0f, 100.0f, 100.0f);
	vertices.emplace_back(100.0f, 100.0f, -100.0f);
	vertices.emplace_back(100.0f, -100.0f, -100.0f);
	vertices.emplace_back(100.0f, -100.0f, -100.0f);
	vertices.emplace_back(100.0f, -100.0f, 100.0f);
	vertices.emplace_back(100.0f, 100.0f, 100.0f);

	// right
	vertices.emplace_back(-100.0f, -100.0f, 100.0f);
	vertices.emplace_back(-100.0f, 100.0f, -100.0f);
	vertices.emplace_back(-100.0f, 100.0f, 100.0f);
	vertices.emplace_back(-100.0f, -100.0f, 100.0f);
	vertices.emplace_back(-100.0f, -100.0f, -100.0f);
	vertices.emplace_back(-100.0f, 100.0f, -100.0f);

	// top
	vertices.emplace_back(-100.0f, 100.0f, -100.0f);
	vertices.emplace_back(100.0f, 100.0f, -100.0f);
	vertices.emplace_back(100.0f, 100.0f, 100.0f);
	vertices.emplace_back(100.0f, 100.0f, 100.0f);
	vertices.emplace_back(-100.0f, 100.0f, 100.0f);
	vertices.emplace_back(-100.0f, 100.0f, -100.0f);

	// bottom
	vertices.emplace_back(-100.0f, -100.0f, -100.0f);
	vertices.emplace_back(-100.0f, -100.0f, 100.0f);
	vertices.emplace_back(100.0f, -100.0f, -100.0f);
	vertices.emplace_back(100.0f, -100.0f, -100.0f);
	vertices.emplace_back(-100.0f, -100.0f, 100.0f);
	vertices.emplace_back(100.0f, -100.0f, 100.0f);

	// back
	vertices.emplace_back(-100.0f, 100.0f, 100.0f);
	vertices.emplace_back(100.0f, 100.0f, 100.0f);
	vertices.emplace_back(100.0f, -100.0f, 100.0f);
	vertices.emplace_back(100.0f, -100.0f, 100.0f);
	vertices.emplace_back(-100.0f, -100.0f, 100.0f);
	vertices.emplace_back(-100.0f, 100.0f, 100.0f);

	// front
	vertices.emplace_back(100.0f, 100.0f, -100.0f);
	vertices.emplace_back(-100.0f, 100.0f, -100.0f);
	vertices.emplace_back(-100.0f, -100.0f, -100.0f);
	vertices.emplace_back(-100.0f, -100.0f, -100.0f);
	vertices.emplace_back(100.0f, -100.0f, -100.0f);
	vertices.emplace_back(100.0f, 100.0f, -100.0f);

	m_VAO.bind();
	m_VAO.SetBuffer<0, GL_ARRAY_BUFFER>(vertices);


	m_VAO.EnableArray<0>();

	m_VAO.unbind();
}

//=================================================================================
void C_SkyBox::AddTexture(E_Side side, const std::filesystem::path& filename)
{
	Renderer::Textures::TextureLoader tl;
	auto							  tex = tl.loadTexture(filename);

	if (!tex)
		CORE_LOG(E_Level::Error, E_Context::Render, "Texture cannot be loaded");

	//m_Textures.bind();
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<int>(side), 0, GL_RGBA, tex->GetDimensions().x, tex->GetDimensions().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex->GetData());

	//m_Textures.unbind();
}

//=================================================================================
void C_SkyBox::PerformDraw() const
{
	auto& tm = Textures::C_TextureUnitManger::Instance();
	//tm.BindTextureToUnit(m_Textures, 0);

	auto& shmgr	 = Shaders::C_ShaderManager::Instance();
	auto  shader = shmgr.GetProgram("skybox");
	shmgr.ActivateShader(shader);

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			glDepthFunc(GL_LEQUAL); // change depth function so depth test passes when values are equal to depth buffer's content

			m_VAO.bind();
			//m_Textures.bind();
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glDepthFunc(GL_LESS); // set depth function back to default
			m_VAO.unbind();
		},
		"SkyBox draw"));
}

//=================================================================================
Physics::Primitives::S_AABB C_SkyBox::GetAABB() const
{
	return m_AABB;
}

//=================================================================================
std::string_view C_SkyBox::GetDebugComponentName() const
{
	return "SkyBox";
}

//=================================================================================
bool C_SkyBox::HasDebugDrawGUI() const
{
	return true;
}

} // namespace GLEngine::GLRenderer::Components