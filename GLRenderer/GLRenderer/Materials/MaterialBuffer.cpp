#include <GLRendererStdafx.h>

#include <GLRenderer/Materials/MaterialBuffer.h>

namespace GLEngine::GLRenderer::Material {

//=================================================================================
C_MaterialsBuffer::C_MaterialsBuffer(const std::string& blockName, unsigned int index)
	: Buffers::C_UniformBuffer(blockName, index)
{
	AllocateMemory(true, &(m_PhongMaterials[0]));
}

//=================================================================================
void C_MaterialsBuffer::UploadData() const
{
	bind();
	constexpr auto phongMaterialsSize = sizeof(m_PhongMaterials);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, phongMaterialsSize, &(m_PhongMaterials[0]));
	unbind();
}

//=================================================================================
std::size_t C_MaterialsBuffer::GetBufferSize() const
{
	return sizeof(m_PhongMaterials);
}

} // namespace GLEngine::GLRenderer::Material
