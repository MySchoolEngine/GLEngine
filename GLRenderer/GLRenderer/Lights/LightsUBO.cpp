#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/LightsUBO.h>


namespace GLEngine::GLRenderer {

//=================================================================================
C_LightsBuffer::C_LightsBuffer(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
{
	m_PointLight.m_Color = glm::vec3(1.0f);
	m_PointLight.m_Position = glm::vec3(0, 2.0f, 0.f);
	m_PointLight.m_Intensity = 20.f;

	constexpr auto pointLightSize = sizeof(m_PointLight);


	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, pointLightSize, &(m_PointLight.m_Position), GL_DYNAMIC_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_LightsBuffer::UploadData() const
{
	bind();

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_PointLight), &(m_PointLight.m_Position));
	// auto* data = (char*)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);
	// 
	// memcpy(data, &m_PointLight, sizeof(m_PointLight));
	// 
	// glUnmapBuffer(GL_UNIFORM_BUFFER);
	unbind();
}

}