#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/ModelData.h>
#include <GLRenderer/GLResourceManager.h>

#include <Renderer/Resources/ResourceManager.h>

namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_ModelData::C_ModelData(const std::string& blockName, unsigned int index, Renderer::ResourceManager& resourceManager)
	: C_UniformBuffer(blockName, index, resourceManager)
	, m_ModelMatrix(glm::mat4(1.f))
	, m_MaterialIndex(0)
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
