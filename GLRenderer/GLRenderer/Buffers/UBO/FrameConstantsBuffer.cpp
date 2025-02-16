#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/FrameConstantsBuffer.h>
#include <GLRenderer/GLResourceManager.h>

#include <Renderer/Resources/ResourceManager.h>

#include <glm/gtc/type_ptr.hpp>


namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_FrameConstantsBuffer::C_FrameConstantsBuffer(const std::string& blockName, unsigned int index, Renderer::ResourceManager& resourceManager)
	: C_UniformBuffer(blockName, index, resourceManager)
	, m_AmbientStrength(0.1f)
	, m_Time(0.0f)
{
	m_Handle = resourceManager.createBuffer(Renderer::BufferDescriptor{.size  = static_cast<uint32_t>(GetBufferSize()),
																	   .type  = Renderer::E_BufferType::Uniform,
																	   .usage = Renderer::E_ResourceUsage::Dynamic,
																	   .name  = blockName});
	// bind buffer base here
	auto& glResourceManager = dynamic_cast<GLResourceManager&>(resourceManager);
	glResourceManager.GetBuffer(m_Handle)->BindBase(GetIndex());
}

//=================================================================================
std::size_t C_FrameConstantsBuffer::GetBufferSize() const
{
	constexpr auto matSize	 = sizeof(glm::mat4);
	constexpr auto vecSize	 = sizeof(glm::vec4);
	constexpr auto floatSize = sizeof(float);

	constexpr auto bytes = 3 * matSize + vecSize + 4 * floatSize;

	return bytes;
}

//=================================================================================
const void* C_FrameConstantsBuffer::Data() const
{
	return &m_ProjectionMat;
}

//=================================================================================
void C_FrameConstantsBuffer::UpdateViewProjection()
{
	m_viewProjectionMatrix = m_ProjectionMat * m_ViewMat;
}

} // namespace GLEngine::GLRenderer::Buffers::UBO