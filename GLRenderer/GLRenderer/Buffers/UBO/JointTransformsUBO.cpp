#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UBO/JointTransformsUBO.h>

#include <glm/gtc/type_ptr.hpp>

namespace GLEngine::GLRenderer::Buffers::UBO {

//=================================================================================
C_JointTramsformsUBO::C_JointTramsformsUBO(const std::string& blockName, unsigned int index, const std::size_t numJoints, Renderer::ResourceManager& resourceManager)
	: C_UniformBuffer(blockName, index, resourceManager)
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
const void* C_JointTramsformsUBO::Data() const
{
	return m_JointTransforms.data();
}

//=================================================================================
void C_JointTramsformsUBO::SetTransforms(std::vector<Renderer::S_BoneKeyframe>&& boneKeyframes)
{
	GLE_ASSERT(m_JointTransforms.size() == boneKeyframes.size(), "You are setting wrong animation data to UBO for different skeleton");
	std::transform(boneKeyframes.begin(), boneKeyframes.end(), m_JointTransforms.begin(),
				   [](const Renderer::S_BoneKeyframe& keyFrame) { return keyFrame.GetTransformationMatrix(); });
}

} // namespace GLEngine::GLRenderer::Buffers::UBO