#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/RainDataBuffer.h>

namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_RainDataBuffer::C_RainDataBuffer(const std::string& blockName, unsigned int index, unsigned int textureDimension)
	: C_UniformBuffer(blockName, index)
	, m_TextureDimension(textureDimension)
{
	AllocateMemory(true, m_RainDrops.data());
}

//=================================================================================
std::size_t C_RainDataBuffer::GetBufferSize() const
{
	const auto bytes = sizeof(m_RainDrops);

	return bytes;
}

//=================================================================================
void C_RainDataBuffer::UploadData() const
{
	bind();
	auto* data = (char*)glMapBuffer(GetBufferType(), GL_WRITE_ONLY);

	memcpy(data, m_RainDrops.data(), GetBufferSize());

	glUnmapBuffer(GetBufferType());
	unbind();
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