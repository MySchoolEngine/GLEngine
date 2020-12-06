#include <GLRendererStdAfx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>

namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_ModelData::C_ModelData(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
	, m_ModelMatrix(glm::mat4(1.f))
	, m_MaterialIndex(0)
{

	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, BufferSize(), nullptr, GL_STREAM_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_ModelData::UploadData() const
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, BufferSize(), &(m_ModelMatrix));
	unbind();
}

//=================================================================================
constexpr std::size_t C_ModelData::BufferSize() const
{
	constexpr auto matSize = sizeof(glm::mat4);
	return matSize + sizeof(m_MaterialIndex);
}

}
