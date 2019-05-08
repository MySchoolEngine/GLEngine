#pragma once

#include <GLRenderer\Buffers\UniformBuffer.h>

namespace GLEngine {
namespace GLRenderer {
namespace Buffers {
namespace UBO {

class C_RainDataBuffer : public C_UniformBuffer {
public:
	C_RainDataBuffer(const std::string& blockName, unsigned int index, unsigned int textureDimension);
	virtual void UploadData() const override;

	void GenerateDrops();

	std::array<glm::ivec2, 100> m_RainDrops;
private:
	unsigned int m_TextureDimension;
};

}}}}