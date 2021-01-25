#include <GLRendererStdafx.h>

#include <GLRenderer/Mesh/TerrainMeshResource.h>


namespace GLEngine::GLRenderer::Mesh {

//=================================================================================
C_TerrainMeshResource::C_TerrainMeshResource()
{
	m_GLVAO.bind();
	m_GLVAO.unbind();
}

//=================================================================================
std::unique_ptr<GLEngine::Renderer::I_RawGPUData> C_TerrainMeshResource::ExtractData() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_TerrainMeshResource::Invalidate()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
bool C_TerrainMeshResource::IsValid()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_TerrainMeshResource::BindVAO() const
{
	m_GLVAO.bind();
}

//=================================================================================
void C_TerrainMeshResource::UnbindVAO() const
{
	m_GLVAO.unbind();
}

//=================================================================================
std::size_t C_TerrainMeshResource::GetNumTriangles() const
{
	return GetWidth() * 2 * (GetHeight() - 1) + GetHeight() - 1;
}

//=================================================================================
std::size_t C_TerrainMeshResource::GetWidth() const
{
	return 4;
}

//=================================================================================
std::size_t C_TerrainMeshResource::GetHeight() const
{
	return 4;
}

} // namespace GLEngine::GLRenderer::Mesh