#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>

namespace GLEngine::GLRenderer::Buffers::UBO {
class C_ModelData : public C_UniformBuffer {
public:
	C_ModelData(const std::string& blockName, unsigned int index);

	virtual void UploadData() const override;

	void SetModelMatrix(const glm::mat4& mat) { m_ModelMatrix = mat; }
	void SetMaterialIndex(int index) { m_MaterialIndex = index; }

	constexpr std::size_t BufferSize() const;

	[[nodiscard]] virtual std::size_t GetBufferSize() const override;

private:
	glm::mat4 m_ModelMatrix;
	int		  m_MaterialIndex;
};
} // namespace GLEngine::GLRenderer::Buffers::UBO