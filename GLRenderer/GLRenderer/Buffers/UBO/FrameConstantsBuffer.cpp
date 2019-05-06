#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>

#include <glm\gtc\type_ptr.hpp>


namespace GLEngine {
namespace GLRenderer {
namespace Buffers {
namespace UBO {

//=================================================================================
C_FrameConstantsBuffer::C_FrameConstantsBuffer(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
{
	const auto matSize = sizeof(glm::mat4);
	const auto vecSize = sizeof(glm::vec4);

	const auto bytes = matSize + vecSize;

//	ErrorCheck();
	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);
	C_UniformBuffer::unbind();
//	ErrorCheck();
}

//=================================================================================
void C_FrameConstantsBuffer::UploadData() const
{
	bind();
	auto *data = (char *)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

	memcpy(data, glm::value_ptr(m_ViewProjection), sizeof(decltype(m_ViewProjection)));
	memcpy(data + sizeof(decltype(m_ViewProjection)), glm::value_ptr(m_CameraPosition), sizeof(decltype(m_CameraPosition)));

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	unbind();
}
}}}}