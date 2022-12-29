#pragma once

#include <Renderer/RendererApi.h>

#include <glm/glm.hpp>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT I_Pose {
public:
	virtual ~I_Pose()																			  = default;
	virtual const glm::mat4		   GetLocalSpaceTransform(int boneID) const						  = 0;
	virtual void				   SetLocalSpaceTransform(const glm::mat4& transform, int boneID) = 0;
	virtual std::vector<glm::mat4> GetLocalSpaceTransofrms() const								  = 0;
};

} // namespace GLEngine::Renderer