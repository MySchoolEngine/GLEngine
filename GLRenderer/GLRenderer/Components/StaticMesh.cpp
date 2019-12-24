#include <GLRendererStdafx.h>

#include <GLRenderer/Components/StaticMesh.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

#include <pugixml.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

//=================================================================================
C_StaticMesh::C_StaticMesh(std::string meshFile)
	: Renderer::I_RenderableComponent(nullptr)
	, m_meshFile(meshFile)
	, m_Mesh(nullptr)
{
	// @todo lazy init
	auto sl = std::make_unique<Mesh::SceneLoader>();

	auto scene = std::make_shared<Mesh::Scene>();
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	if (!sl->addModelFromFileToScene("Models", m_meshFile.c_str(), scene, modelMatrix))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to load model {}", "Models/scene.obj");
		return;
	}

	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(scene->meshes[0]);
}

//=================================================================================
C_StaticMesh::C_StaticMesh(const Mesh::Mesh& mesh)
	: Renderer::I_RenderableComponent(nullptr)
{
	m_Mesh = std::make_shared<Mesh::C_StaticMeshResource>(mesh);
}

//=================================================================================
void C_StaticMesh::PerformDraw() const
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh)
		)
	);
}

//=================================================================================
std::shared_ptr<Entity::I_Component> C_StaticMeshBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	return std::make_shared<C_StaticMesh>(node.attribute("filePath").value());
}

}}}