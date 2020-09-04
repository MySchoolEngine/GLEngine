#include <GLRendererStdAfx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>

namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_ModelData::C_ModelData(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
	, m_ModelMatrix(glm::mat4(1.f))
	, m_MaterialIndex(0)
{
	const auto matSize = sizeof(glm::mat4);

	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, matSize + sizeof(m_MaterialIndex), nullptr, GL_DYNAMIC_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_ModelData::UploadData() const
{
	bind();
	constexpr auto bufferSize = sizeof(m_ModelMatrix) + sizeof(m_MaterialIndex);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, bufferSize, &(m_ModelMatrix));
	unbind();
}

}
