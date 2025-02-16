#include <GLRendererStdafx.h>

#include <GLRenderer/Materials/MaterialBuffer.h>
#include <GLRenderer/GLResourceManager.h>

#include <Renderer/Resources/ResourceManager.h>

namespace GLEngine::GLRenderer::Material {

//=================================================================================
C_MaterialsBuffer::C_MaterialsBuffer(const std::string& blockName, unsigned int index, Renderer::ResourceManager& resourceManager)
	: Buffers::C_UniformBuffer(blockName, index, resourceManager)
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
std::size_t C_MaterialsBuffer::GetBufferSize() const
{
	return sizeof(m_PhongMaterials);
}

//=================================================================================
const void* C_MaterialsBuffer::Data() const
{
	return m_PhongMaterials.data();
}

} // namespace GLEngine::GLRenderer::Material
