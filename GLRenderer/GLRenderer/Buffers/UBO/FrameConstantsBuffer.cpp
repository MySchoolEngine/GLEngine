#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>

#include <glm/gtc/type_ptr.hpp>


namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_FrameConstantsBuffer::C_FrameConstantsBuffer(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
	, m_AmbientStrength(0.1f)
	, m_Time(0.0f)
{
	const auto matSize	 = sizeof(glm::mat4);
	const auto vecSize	 = sizeof(glm::vec4);
	const auto vec3Size	 = sizeof(glm::vec3);
	const auto floatSize = sizeof(float);

	const auto bytes = 3 * matSize + vecSize + vec3Size + floatSize;

	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_STATIC_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_FrameConstantsBuffer::UploadData() const
{
	const auto matSize			 = sizeof(glm::mat4);
	const auto vecSize			 = sizeof(glm::vec4);
	const auto vec3Size			 = sizeof(glm::vec3);
	const auto viewProjectionMat = m_ProjectionMat * m_ViewMat;
	const auto floatSize		 = sizeof(float);
	bind();
	auto* data = (char*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

	memcpy(data, glm::value_ptr(m_ProjectionMat), matSize);
	memcpy(data + matSize, glm::value_ptr(m_ViewMat), matSize);
	memcpy(data + 2 * matSize, glm::value_ptr(viewProjectionMat), matSize);
	memcpy(data + 3 * matSize, glm::value_ptr(m_CameraPosition), sizeof(decltype(m_CameraPosition)));
	memcpy(data + 3 * matSize + vecSize, &m_AmbientStrength, sizeof(decltype(m_AmbientStrength)));
	memcpy(data + 3 * matSize + vecSize + sizeof(decltype(m_AmbientStrength)), &m_Time, sizeof(decltype(m_Time)));
	memcpy(data + 3 * matSize + vecSize + sizeof(decltype(m_AmbientStrength)) + floatSize, &m_NearPlane, sizeof(decltype(m_NearPlane)));
	memcpy(data + 3 * matSize + vecSize + sizeof(decltype(m_AmbientStrength)) + 2 * floatSize, &m_FarPlane, sizeof(decltype(m_FarPlane)));

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	unbind();
}
} // namespace GLEngine::GLRenderer::Buffers::UBO