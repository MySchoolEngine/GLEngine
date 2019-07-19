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

	const auto bytes = 3*matSize + vecSize;

	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_FrameConstantsBuffer::UploadData() const
{
	const auto matSize = sizeof(glm::mat4);
	const auto viewProjectionMat = m_ProjectionMat*m_ViewMat;
	bind();
	auto *data = (char *)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

	memcpy(data, glm::value_ptr(m_ProjectionMat), matSize);
	memcpy(data + matSize, glm::value_ptr(m_ViewMat), matSize);
	memcpy(data + 2*matSize, glm::value_ptr(viewProjectionMat), matSize);
	memcpy(data + 3*matSize, glm::value_ptr(m_CameraPosition), sizeof(decltype(m_CameraPosition)));

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	unbind();
}
}}}}