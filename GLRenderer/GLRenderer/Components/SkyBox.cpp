#include <GLRendererStdafx.h>

#include <GLRenderer/Components/SkyBox.h>

#include <GLRenderer/Buffers/GLBuffer.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <Renderer/Mesh/Scene.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <GLRenderer/CameraManager.h>
#include <Renderer/ICameraComponent.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

#include <pugixml.hpp>

namespace GLEngine::GLRenderer::Components {

//=================================================================================
C_SkyBox::C_SkyBox()
	: m_Textures("Skybox", GL_TEXTURE_CUBE_MAP)
{
	m_Textures.SetFilter(GL_LINEAR, GL_LINEAR);
	m_Textures.SetWrap(E_WrapFunction::ClampToEdge, E_WrapFunction::ClampToEdge, E_WrapFunction::ClampToEdge);

	std::vector<glm::vec3> vertices;
	// left
	vertices.emplace_back(100.0f,  100.0f,  100.0f);
	vertices.emplace_back(100.0f,  100.0f, -100.0f);
	vertices.emplace_back(100.0f, -100.0f, -100.0f);
	vertices.emplace_back(100.0f, -100.0f, -100.0f);
	vertices.emplace_back(100.0f, -100.0f,  100.0f);
	vertices.emplace_back(100.0f,  100.0f,  100.0f);

	// right
	vertices.emplace_back(-100.0f, -100.0f,  100.0f);
	vertices.emplace_back(-100.0f,  100.0f, -100.0f);
	vertices.emplace_back(-100.0f,  100.0f,  100.0f);
	vertices.emplace_back(-100.0f, -100.0f,  100.0f);
	vertices.emplace_back(-100.0f, -100.0f, -100.0f);
	vertices.emplace_back(-100.0f,  100.0f, -100.0f);
	
	// top
	vertices.emplace_back(-100.0f, 100.0f, -100.0f);
	vertices.emplace_back( 100.0f, 100.0f, -100.0f);
	vertices.emplace_back( 100.0f, 100.0f,  100.0f);
	vertices.emplace_back( 100.0f, 100.0f,  100.0f);
	vertices.emplace_back(-100.0f, 100.0f,  100.0f);
	vertices.emplace_back(-100.0f, 100.0f, -100.0f);

	// bottom
	vertices.emplace_back(-100.0f, -100.0f, -100.0f);
	vertices.emplace_back(-100.0f, -100.0f,  100.0f);
	vertices.emplace_back( 100.0f, -100.0f, -100.0f);
	vertices.emplace_back( 100.0f, -100.0f, -100.0f);
	vertices.emplace_back(-100.0f, -100.0f,  100.0f);
	vertices.emplace_back( 100.0f, -100.0f,  100.0f);

	//back
	vertices.emplace_back(-100.0f,  100.0f, 100.0f);
	vertices.emplace_back( 100.0f,  100.0f, 100.0f);
	vertices.emplace_back( 100.0f, -100.0f, 100.0f);
	vertices.emplace_back( 100.0f, -100.0f, 100.0f);
	vertices.emplace_back(-100.0f, -100.0f, 100.0f);
	vertices.emplace_back(-100.0f,  100.0f, 100.0f);

	//front
	vertices.emplace_back( 100.0f,  100.0f, -100.0f);
	vertices.emplace_back(-100.0f,  100.0f, -100.0f);
	vertices.emplace_back(-100.0f, -100.0f, -100.0f);
	vertices.emplace_back(-100.0f, -100.0f, -100.0f);
	vertices.emplace_back( 100.0f, -100.0f, -100.0f);
	vertices.emplace_back( 100.0f,  100.0f, -100.0f);

	m_VAO.bind();
	m_VAO.SetBuffer<0, GL_ARRAY_BUFFER>(vertices);


	m_VAO.EnableArray<0>();

	m_VAO.unbind();
}

//=================================================================================
void C_SkyBox::AddTexture(E_Side side, const std::string& filename)
{
	Textures::TextureLoader tl;
	Renderer::MeshData::Texture t;
	bool retval = tl.loadTexture(filename.c_str(), t);

	if (!retval)
		CORE_LOG(E_Level::Error, E_Context::Render, "Texture cannot be loaded");


	const int mipMapLevel = 0;
	m_Textures.bind();
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<int>(side), 0, GL_RGBA, t.width, t.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, t.data.get());

	m_Textures.GenerateMipMaps();
	m_Textures.unbind();
}

//=================================================================================
void C_SkyBox::PerformDraw() const
{
	auto& tm = Textures::C_TextureUnitManger::Instance();
	tm.BindTextureToUnit(m_Textures, 0);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
				[&]() {
					glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
					auto& shmgr = Shaders::C_ShaderManager::Instance();
					auto shader = shmgr.GetProgram("skybox");
					shmgr.ActivateShader(shader);

					m_VAO.bind();
					glBindTexture(GL_TEXTURE_CUBE_MAP, m_Textures.GetTexture());
					glDrawArrays(GL_TRIANGLES, 0, 36);
					glDepthFunc(GL_LESS); // set depth function back to default
					m_VAO.unbind();
				}
			)
		)
	);
}

//=================================================================================
// C_SkyBoxCompBuilder
//=================================================================================
std::shared_ptr<Entity::I_Component> C_SkyBoxCompBuilder::Build(const pugi::xml_node& node)
{
	auto skyboxComp = std::make_shared<C_SkyBox>();

	if (auto side = node.child("Top"))
	{
		auto& attrib = side.attribute("image");
		skyboxComp->AddTexture(C_SkyBox::E_Side::Top, attrib.value());
	}
	if (auto side = node.child("Bottom"))
	{
		auto& attrib = side.attribute("image");
		skyboxComp->AddTexture(C_SkyBox::E_Side::Bottom, attrib.value());
	}
	if (auto side = node.child("Left"))
	{
		auto& attrib = side.attribute("image");
		skyboxComp->AddTexture(C_SkyBox::E_Side::Left, attrib.value());
	}
	if (auto side = node.child("Right"))
	{
		auto& attrib = side.attribute("image");
		skyboxComp->AddTexture(C_SkyBox::E_Side::Right, attrib.value());
	}
	if (auto side = node.child("Back"))
	{
		auto& attrib = side.attribute("image");
		skyboxComp->AddTexture(C_SkyBox::E_Side::Back, attrib.value());
	}
	if (auto side = node.child("Forward"))
	{
		auto& attrib = side.attribute("image");
		skyboxComp->AddTexture(C_SkyBox::E_Side::Forward, attrib.value());
	}

	return skyboxComp;
}

}