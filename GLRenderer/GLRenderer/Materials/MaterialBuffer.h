#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>
#include <GLRenderer/Materials/PhongMaterial.h>

namespace GLEngine::GLRenderer::Material {
class C_MaterialsBuffer final : public Buffers::C_UniformBuffer {
public:
	C_MaterialsBuffer(const std::string& blockName, unsigned int index);
	virtual void UploadData() const override;

	[[nodiscard]] virtual std::size_t GetBufferSize() const override;

	std::array<C_PhongMaterial, 40> m_PhongMaterials;
};
} // namespace GLEngine::GLRenderer::Material