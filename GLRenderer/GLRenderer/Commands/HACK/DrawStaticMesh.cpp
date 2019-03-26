#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>


namespace GLEngine {
namespace GLRenderer {
namespace Commands {
namespace HACK {

//=================================================================================
C_DrawStaticMesh::C_DrawStaticMesh(std::shared_ptr<Mesh::C_StaticMeshResource> mesh)
	: m_Mesh(mesh)
{}

//=================================================================================
void C_DrawStaticMesh::Commit()
{
	glBindVertexArray(m_Mesh->GetVAO());
	glDrawArrays(GL_TRIANGLES, 0, m_Mesh->GetNumTriangles());
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_DrawStaticMesh::GetType() const
{
	// as this whole command is big hack, this doesn't matter
	return Renderer::I_RenderCommand::E_Type::DrawCall;
}

//=================================================================================
std::shared_ptr<Renderer::I_Resource> C_DrawStaticMesh::GetResource() const
{
	return m_Mesh;
}

}}}}