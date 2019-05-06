#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/RainDataBuffer.h>

#include <glm\gtc\type_ptr.hpp>


namespace GLEngine {
namespace GLRenderer {
namespace Buffers {
namespace UBO {

//=================================================================================
C_RainDataBuffer::C_RainDataBuffer(const std::string& blockName, unsigned int index, unsigned int textureDimension)
	: C_UniformBuffer(blockName, index)
	, m_TextureDimension(textureDimension)
{
	const auto bytes = sizeof(m_RainDrops);

	C_UniformBuffer::bind();
	glBufferData(GL_UNIFORM_BUFFER, bytes, nullptr, GL_DYNAMIC_DRAW);
	C_UniformBuffer::unbind();
}

//=================================================================================
void C_RainDataBuffer::UploadData() const
{
	bind();
	auto *data = (char *)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

	memcpy(data, m_RainDrops.data(), sizeof(m_RainDrops));

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	unbind();
}

//=================================================================================
void C_RainDataBuffer::GenerateDrops()
{
	unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine generator(seed1);
	std::uniform_int_distribution<int> distribution(0, m_TextureDimension);
	for (int i = 0; i < m_RainDrops.size(); ++i) {
		m_RainDrops[i].x = distribution(generator);
		m_RainDrops[i].y = distribution(generator);
	}
}

}}}}