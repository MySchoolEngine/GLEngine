#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/LightsUBO.h>


namespace GLEngine::GLRenderer {

//=================================================================================
C_LightsBuffer::C_LightsBuffer(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
{

	constexpr auto pointLightSize = sizeof(m_PointLight);


	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, pointLightSize, &(m_PointLight[0].m_Position), GL_DYNAMIC_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_LightsBuffer::UploadData() const
{
	bind();

	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(m_PointLight), &(m_PointLight[0].m_Position));
	// auto* data = (char*)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);
	// 
	// memcpy(data, &m_PointLight, sizeof(m_PointLight));
	// 
	// glUnmapBuffer(GL_UNIFORM_BUFFER);
	unbind();
}

}