#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/DrawStaticMesh.h>
#include <GLRenderer/Mesh/StaticMeshResource.h>


namespace GLEngine::GLRenderer::Commands::HACK {

//=================================================================================
C_DrawStaticMesh::C_DrawStaticMesh(std::shared_ptr<Mesh::C_StaticMeshResource> mesh)
	: m_Mesh(mesh)
{
}

//=================================================================================
void C_DrawStaticMesh::Commit()
{
	m_Mesh->BindVAO();
	glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(m_Mesh->GetNumTriangles()));
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_DrawStaticMesh::GetType() const
{
	// as this whole command is big hack, this doesn't matter
	return Renderer::I_RenderCommand::E_Type::DrawCall;
}

//=================================================================================
std::string C_DrawStaticMesh::GetDescriptor() const
{
	return fmt::format("DrawStaticMesh tris: {}", m_Mesh->GetNumTriangles());
}

} // namespace GLEngine::GLRenderer::Commands::HACK