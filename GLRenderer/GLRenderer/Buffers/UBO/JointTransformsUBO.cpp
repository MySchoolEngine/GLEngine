#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/JointTransformsUBO.h>

#include <glm/gtc/type_ptr.hpp>

namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_JointTramsformsUBO::C_JointTramsformsUBO(const std::string& blockNamem, unsigned int index, const std::size_t numJoints)
	: C_UniformBuffer(blockNamem, index)
{
	m_JointTransforms.resize(numJoints);
}

//=================================================================================
std::size_t C_JointTramsformsUBO::GetBufferSize() const
{
	// TODO: This is wrong size
	constexpr auto matSize = sizeof(glm::mat4);

	return matSize;
}

//=================================================================================
void C_JointTramsformsUBO::SetTransforms(std::vector<Renderer::S_BoneKeyframe>&& boneKeyframes)
{
	GLE_ASSERT(m_JointTransforms.size() == boneKeyframes.size(), "You are setting wrong animation data to UBO for different skeleton");
	std::transform(boneKeyframes.begin(), boneKeyframes.end(), m_JointTransforms.begin(),
				   [](const Renderer::S_BoneKeyframe& keyFrame) { return keyFrame.GetTransformationMatrix(); });
}

//=================================================================================
void C_JointTramsformsUBO::UploadData() const
{
	constexpr auto matSize = sizeof(glm::mat4);
	bind();
	auto* data = (char*)glMapBuffer(GL_UNIFORM_BUFFER, GL_READ_WRITE);

	// well, this isn't really "safest" way to copy all transforms
	// BUT matrix data are stored as first data member of the glm::mat4
	// AND std::vector should store them in continuous block of memory
	// so it is as good as copying one by one, but way faster
	memcpy(data, glm::value_ptr(m_JointTransforms[0]), matSize);

	glUnmapBuffer(GL_UNIFORM_BUFFER);
	unbind();
}

} // namespace GLEngine::GLRenderer::Buffers::UBO