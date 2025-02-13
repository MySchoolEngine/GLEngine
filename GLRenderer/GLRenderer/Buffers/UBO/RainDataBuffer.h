#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>

namespace GLEngine::GLRenderer::Buffers::UBO {

class C_RainDataBuffer final : public C_UniformBuffer {
public:
	C_RainDataBuffer(const std::string& blockName, unsigned int index, unsigned int textureDimension);
	void					  UploadData() const override;
	[[nodiscard]] std::size_t GetBufferSize() const override;

	void GenerateDrops();

	std::array<glm::ivec2, 100> m_RainDrops;

private:
	unsigned int m_TextureDimension;
};

} // namespace GLEngine::GLRenderer::Buffers::UBO