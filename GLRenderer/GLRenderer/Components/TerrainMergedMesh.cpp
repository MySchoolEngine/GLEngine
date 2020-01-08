#include <GLRendererStdafx.h>

#include <GLRenderer/Components/TerrainMergedMesh.h>


namespace GLEngine {
namespace GLRenderer {
namespace Components {

//=================================================================================
C_TerrainMergedMesh::C_TerrainMergedMesh(std::vector<std::shared_ptr<C_TerrainMesh>> meshes)
	: Renderer::I_RenderableComponent(nullptr)
	, m_Meshes(meshes)
{

}

//=================================================================================
void C_TerrainMergedMesh::PerformDraw() const
{
	std::for_each(m_Meshes.begin(), m_Meshes.end(), [](std::shared_ptr<C_TerrainMesh> mesh) {
		mesh->PerformDraw();
	});
}

}}}