#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>

#include <Renderer/Animation/SkeletalAnimation.h>

namespace GLEngine::GLRenderer::Buffers::UBO {
class C_JointTramsformsUBO : public C_UniformBuffer {
public:
	C_JointTramsformsUBO(const std::string& blockName, unsigned int index, const std::size_t numJoints, Renderer::ResourceManager& resourceManager);
	void SetTransforms(std::vector<Renderer::S_BoneKeyframe>&& boneKeyframes);


	[[nodiscard]] std::size_t GetBufferSize() const override;
	const void*				  Data() const override;

private:
	std::vector<glm::mat4> m_JointTransforms;
};
} // namespace GLEngine::GLRenderer::Buffers::UBO
