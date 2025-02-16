#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>
#include <GLRenderer/Materials/PhongMaterial.h>

namespace GLEngine::Renderer {
class I_Renderer;
class ResourceManager;
}

namespace GLEngine::GLRenderer::Material {
class C_MaterialsBuffer final : public Buffers::C_UniformBuffer {
public:
	C_MaterialsBuffer(const std::string& blockName, unsigned int index, Renderer::ResourceManager& resourceManager);

	[[nodiscard]] std::size_t GetBufferSize() const override;
	const void*				  Data() const override;

	std::array<C_PhongMaterial, 40> m_PhongMaterials;
};
} // namespace GLEngine::GLRenderer::Material