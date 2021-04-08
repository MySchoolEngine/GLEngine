#include <GLRendererStdafx.h>

#include <GLRenderer/Materials/MaterialBuffer.h>

namespace GLEngine::GLRenderer::Material {

//=================================================================================
C_MaterialsBuffer::C_MaterialsBuffer(const std::string& blockName, unsigned int index)
	: Buffers::C_UniformBuffer(blockName, index)
{
	constexpr auto phongMaterialsSize = sizeof(m_PhongMaterials);

	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, phongMaterialsSize, &(m_PhongMaterials[0]), GL_DYNAMIC_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_MaterialsBuffer::UploadData() const
{
	bind();
	constexpr auto phongMaterialsSize = sizeof(m_PhongMaterials);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, phongMaterialsSize, &(m_PhongMaterials[0]));
	unbind();
}

} // namespace GLEngine::GLRenderer::Material
