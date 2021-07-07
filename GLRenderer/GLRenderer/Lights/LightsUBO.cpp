#include <GLRendererStdafx.h>

#include <GLRenderer/Lights/LightsUBO.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureLoader.h>


namespace GLEngine::GLRenderer {

//=================================================================================
C_LightsBuffer::C_LightsBuffer(const std::string& blockName, unsigned int index)
	: C_UniformBuffer(blockName, index)
{
	AllocateMemory(true, &(m_PointLight[0].m_Position));

	Textures::TextureLoader tl;

	m_LTCFittingTexture = tl.LoadAndInitTexture("Images\\ltc_1.dds");
	if (!m_LTCFittingTexture)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Cannot load ltc mat");
		return;
	}
	m_LTCFittingTexture->bind();
	m_LTCFittingSchemeHandle = m_LTCFittingTexture->CreateHandle();
	m_LTCFittingTexture->unbind();


	m_LTCMagTexture = tl.LoadAndInitTexture("Images\\ltc_2.dds");
	if (!m_LTCMagTexture)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Cannot load ltc mat");
		return;
	}
	m_LTCMagTexture->bind();
	m_LTCMagSchemeHandle = m_LTCMagTexture->CreateHandle();
	m_LTCMagTexture->unbind();
}

//=================================================================================
std::size_t C_LightsBuffer::GetBufferSize() const
{
	constexpr auto pointLightSize = sizeof(m_PointLight);
	constexpr auto areaLightSize  = sizeof(m_AreaLight);
	constexpr auto sunLightSize	  = sizeof(m_SunLight);
	constexpr auto handleSize	  = sizeof(std::uint64_t);

	constexpr auto bytes = pointLightSize + areaLightSize + sunLightSize + handleSize * 2;
	return bytes;
}

//=================================================================================
void C_LightsBuffer::UploadData() const
{
	bind();
	glBufferSubData(GL_UNIFORM_BUFFER, 0, GetBufferSize(), &(m_PointLight[0].m_Position));
	unbind();
}

//=================================================================================
void C_LightsBuffer::MakeHandlesResident(bool val /*= true*/)
{
	m_LTCFittingTexture->MakeHandleResident(val);
	m_LTCMagTexture->MakeHandleResident(val);
}

} // namespace GLEngine::GLRenderer