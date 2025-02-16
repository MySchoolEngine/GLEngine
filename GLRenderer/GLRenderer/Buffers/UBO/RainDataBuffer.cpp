#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/RainDataBuffer.h>

#include <Renderer/Resources/ResourceManager.h>

namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_RainDataBuffer::C_RainDataBuffer(const std::string& blockName, unsigned int index, unsigned int textureDimension, Renderer::ResourceManager& resourceManager)
	: C_UniformBuffer(blockName, index, resourceManager)
	, m_TextureDimension(textureDimension)
{
	m_Handle = resourceManager.createBuffer(Renderer::BufferDescriptor{.size  = static_cast<uint32_t>(GetBufferSize()),
																	   .type  = Renderer::E_BufferType::Uniform,
																	   .usage = Renderer::E_ResourceUsage::Dynamic,
																	   .name  = blockName});
}

//=================================================================================
std::size_t C_RainDataBuffer::GetBufferSize() const
{
	constexpr auto bytes = sizeof(m_RainDrops);

	return bytes;
}

//=================================================================================
const void* C_RainDataBuffer::Data() const
{
	return m_RainDrops.data();
}

//=================================================================================
void C_RainDataBuffer::GenerateDrops()
{
	unsigned						   seed1 = static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count());
	std::default_random_engine		   generator(seed1);
	std::uniform_int_distribution<int> distribution(0, m_TextureDimension);
	for (int i = 0; i < m_RainDrops.size(); ++i)
	{
		m_RainDrops[i].x = distribution(generator);
		m_RainDrops[i].y = distribution(generator);
	}
}

} // namespace GLEngine::GLRenderer::Buffers::UBO