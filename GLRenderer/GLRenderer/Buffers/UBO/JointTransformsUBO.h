#pragma once

#include <GLRenderer/Buffers/UniformBuffer.h>

#include <Renderer/Animation/SkeletalAnimation.h>

namespace GLEngine::GLRenderer::Buffers::UBO {
class C_JointTramsformsUBO : public C_UniformBuffer {
public:
	C_JointTramsformsUBO(const std::string& blockNamem, unsigned int index, const std::size_t numJoints);
	void SetTransforms(std::vector<Renderer::Animation::S_BoneKeyframe>&& boneKeyframes);


	virtual void UploadData() const override;

private:
	std::vector<glm::mat4> m_JointTransforms;
};
} // namespace GLEngine::GLRenderer::Buffers::UBO
