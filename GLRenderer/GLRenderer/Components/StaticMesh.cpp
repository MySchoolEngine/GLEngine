#include <GLRendererStdafx.h>

#include <GLRenderer/Components/StaticMesh.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Mesh/Scene.h>

#include <Core/Application.h>

#include <pugixml.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

//=================================================================================
C_StaticMesh::C_StaticMesh(std::string meshFile, std::string_view shader)
	: Renderer::I_RenderableComponent(nullptr)
	, m_meshFile(meshFile)
	, m_Mesh(nullptr)
	, m_Color("Model color", glm::vec3(1, 0, 0))
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
	, m_Color("Model color", glm::vec3(1,0,0))
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
		//CORE_LOG(E_Level::Error, E_Context::Render, "Static mesh uncomplete");
		return;
	}

	auto& shmgr = Shaders::C_ShaderManager::Instance();
	shmgr.ActivateShader(m_Shader);

	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_LambdaCommand>(
					[&]() {
						m_Shader->SetUniform("modelMatrix", m_ModelMatrix);
						m_Shader->SetUniform("modelColor", m_Color.GetValue());
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
		std::stringstream ss;
		ss << colorChild.child_value();
		float r, b, g;
		ss >> r >> b >> g;
		staticMesh->SetColor(glm::vec3(r, b, g));
	}

	return staticMesh;
}

}}}