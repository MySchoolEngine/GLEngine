#include <GLRendererStdafx.h>

#include <GLRenderer/Materials/MaterialBuffer.h>

namespace GLEngine::GLRenderer::Material {

//=================================================================================
C_MaterialsBuffer::C_MaterialsBuffer(const std::string& blockName, unsigned int index)
	: Buffers::C_UniformBuffer(blockName, index)
{
}

//=================================================================================
std::size_t C_MaterialsBuffer::GetBufferSize() const
{
	return sizeof(m_PhongMaterials);
}

//=================================================================================
const void* C_MaterialsBuffer::Data() const
{
	return m_PhongMaterials.data();
}

} // namespace GLEngine::GLRenderer::Material
