#include <GLRendererStdafx.h>

#include <GLRenderer/Components/StaticMesh.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>
#include <GLRenderer/Textures/TextureLoader.h>
#include <GLRenderer/Textures/TextureUnitManager.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Application.h>

#include <Utils/Parsing/ColorParsing.h>

#include <pugixml.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

//=================================================================================
C_StaticMesh::C_StaticMesh(std::string meshFile, std::string_view shader)
	: Renderer::I_RenderableComponent(nullptr)
	, m_meshFile(meshFile)
	, m_Mesh(nullptr)
{
	// @todo lazy init
	auto sl = std::make_unique<Mesh::SceneLoader>();

	auto scene = std::make_shared<Renderer::MeshData::Scene>();
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (!sl->addModelFromFileToScene("Models", m_meshFile.c_str(), scene, modelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}", m_meshFile);
		return;
	}

	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(scene->meshes[0]);
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader = shmgr.GetProgram(shader.data());
}

//=================================================================================
C_StaticMesh::C_StaticMesh(const Renderer::MeshData::Mesh& mesh, std::string_view shader)
	: Renderer::I_RenderableComponent(nullptr)
{
	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(mesh);

	auto& shmgr = Shaders::C_ShaderManager::Instance();
	m_Shader = shmgr.GetProgram(shader.data());
}

//=================================================================================
void C_StaticMesh::PerformDraw() const
{
	if (!m_Mesh || !m_Shader)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Static mesh uncomplete");
		return;
	}

	auto& shmgr = Shaders::C_ShaderManager::Instance();
	shmgr.ActivateShader(m_Shader);

	auto& tm = Textures::C_TextureUnitManger::Instance();

	if (m_RoughnessMap) {
		tm.BindTextureToUnit(*m_RoughnessMap, 0);
	}
	if (m_ColorMap) {
		tm.BindTextureToUnit(*m_ColorMap, 1);
	}
	if (m_NormalMap) {
		tm.BindTextureToUnit(*m_NormalMap, 2);
	}

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
					[&]() {
						m_Shader->SetUniform("modelMatrix", m_ModelMatrix);
						m_Shader->SetUniform("modelColor", m_Color.GetValue());
						m_Shader->SetUniform("roughness", m_Roughness.GetValue());
						m_Shader->SetUniform("roughnessMap", 0);
						m_Shader->SetUniform("useRoughnessMap", m_RoughnessMap != nullptr);
						m_Shader->SetUniform("colorMap", 1);
						m_Shader->SetUniform("useColorMap", m_RoughnessMap != nullptr);
						m_Shader->SetUniform("normalMap", 2);
						m_Shader->SetUniform("useNormalMap", m_RoughnessMap != nullptr);
					}
				)
		)
	);


	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh)
		)
	);
}

//=================================================================================
void C_StaticMesh::DebugDrawGUI()
{
	if (::ImGui::CollapsingHeader("Static mesh")) {
		m_Color.Draw();
		if (!m_RoughnessMap)
		{
			m_Roughness.Draw();
		}
	}
}

//=================================================================================
std::shared_ptr<Entity::I_Component> C_StaticMeshBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	std::string_view material = "basic";
	if (auto materialAttr = node.attribute("material"))
	{
		material = materialAttr.value();
	}

	auto staticMesh = std::make_shared<C_StaticMesh>(node.attribute("filePath").value(), material);

	if (auto colorChild = node.child("color"))
	{
		staticMesh->SetColor(Utils::Parsing::C_ColorParser::ParseColorRGB(node));
	}

	if (auto roughness = node.child("roughness"))
	{
		std::stringstream ss;
		ss << roughness.child_value();
		float val;
		ss >> val;
		staticMesh->m_Roughness = val;
	}


	auto& tmgr = Textures::C_TextureManager::Instance();

	if (auto roughnessMap = node.child("roughnessMap"))
	{
		staticMesh->m_RoughnessMap = tmgr.GetTexture(roughnessMap.child_value());
		if (staticMesh->m_RoughnessMap)
		{
			staticMesh->m_Roughness = 1.0f;

			staticMesh->m_RoughnessMap->StartGroupOp();
			staticMesh->m_RoughnessMap->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
			staticMesh->m_RoughnessMap->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			staticMesh->m_RoughnessMap->GenerateMipMaps();

			staticMesh->m_RoughnessMap->EndGroupOp();
		}
	}

	if (auto colorMap = node.child("colorMap"))
	{
		staticMesh->m_ColorMap = tmgr.GetTexture(colorMap.child_value());
		if (staticMesh->m_ColorMap)
		{
			staticMesh->SetColor(glm::vec3(1.0f));

			staticMesh->m_ColorMap->StartGroupOp();
			staticMesh->m_ColorMap->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
			staticMesh->m_ColorMap->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			staticMesh->m_ColorMap->GenerateMipMaps();

			staticMesh->m_ColorMap->EndGroupOp();
		}
	}


	if (auto normalMap = node.child("normalMap"))
	{
		staticMesh->m_NormalMap = tmgr.GetTexture(normalMap.child_value());
		if (staticMesh->m_NormalMap)
		{
			staticMesh->m_NormalMap->StartGroupOp();
			staticMesh->m_NormalMap->SetWrap(E_WrapFunction::Repeat, E_WrapFunction::Repeat);
			staticMesh->m_NormalMap->SetFilter(GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
			staticMesh->m_NormalMap->GenerateMipMaps();

			staticMesh->m_NormalMap->EndGroupOp();
		}
	}

	return staticMesh;
}

}}}