#include <GLRendererStdafx.h>

#include <GLRenderer/Mesh/StaticMeshResource.h>

#include <Renderer/Mesh/Scene.h>

namespace GLEngine::GLRenderer::Mesh {

//=================================================================================
C_StaticMeshResource::C_StaticMeshResource(const Renderer::MeshData::Mesh& mesh)
	: m_Name(mesh.m_name)
{
	static_assert(sizeof(glm::vec3) == sizeof(GLfloat) * 3, "Platform doesn't support this directly.");

	m_triangles = mesh.vertices.size();

	m_VAO.bind();
	m_VAO.SetBuffer<0, GL_ARRAY_BUFFER>(mesh.vertices);
	m_VAO.SetBuffer<1, GL_ARRAY_BUFFER>(mesh.normals);
	m_VAO.SetBuffer<2, GL_ARRAY_BUFFER>(mesh.texcoords);
	m_VAO.SetBuffer<3, GL_ARRAY_BUFFER>(mesh.tangent);
	m_VAO.SetBuffer<4, GL_ARRAY_BUFFER>(mesh.bitangent);


	m_VAO.EnableArray<0>();
	m_VAO.EnableArray<1>();
	m_VAO.EnableArray<2>();
	m_VAO.EnableArray<3>();
	m_VAO.EnableArray<4>();

	m_VAO.unbind();
}

//=================================================================================
std::unique_ptr<GLEngine::Renderer::I_RawGPUData> C_StaticMeshResource::ExtractData() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_StaticMeshResource::Invalidate()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
bool C_StaticMeshResource::IsValid()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_StaticMeshResource::BindVAO() const
{
	m_VAO.bind();
}

//=================================================================================
std::size_t C_StaticMeshResource::GetNumTriangles() const
{
	return m_triangles;
}

} // namespace GLEngine::GLRenderer::Mesh