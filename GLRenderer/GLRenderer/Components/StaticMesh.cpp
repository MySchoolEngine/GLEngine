#include <GLRendererStdafx.h>

#include <GLRenderer/Components/StaticMesh.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>
#include <GLRenderer/MeshLoading/SceneLoader.h>
#include <GLRenderer/MeshLoading/Scene.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

//=================================================================================
C_StaticMesh::C_StaticMesh(std::string meshFile)
	: m_meshFile(meshFile)
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
void C_StaticMesh::PerformDraw() const
{
	Core::C_Application::Get().GetActiveRenderer()->AddCommand(
		std::move(
			std::make_unique<Commands::HACK::C_DrawStaticMesh>(m_Mesh)
		)
	);
}

}}}