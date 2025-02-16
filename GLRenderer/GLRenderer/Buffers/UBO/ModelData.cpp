#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>

namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_ModelData::C_ModelData(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
	, m_ModelMatrix(glm::mat4(1.f))
	, m_MaterialIndex(0)
{}

//=================================================================================
constexpr std::size_t C_ModelData::BufferSize() const
{
	constexpr auto matSize = sizeof(glm::mat4);
	return matSize + sizeof(m_MaterialIndex);
}

//=================================================================================
std::size_t C_ModelData::GetBufferSize() const
{
	return BufferSize();
}

//=================================================================================
const void* C_ModelData::Data() const
{
	return &(m_ModelMatrix);
}

} // namespace GLEngine::GLRenderer::Buffers::UBO
