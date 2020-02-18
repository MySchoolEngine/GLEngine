#pragma once

#include <Renderer/RendererApi.h>

#include <glm/glm.hpp>

namespace GLEngine::Renderer::Animation {

class RENDERER_API_EXPORT I_Pose {
public:
	virtual ~I_Pose() = default;
	virtual const glm::mat4 GetModelSpaceTransform(int boneID) const = 0;
	virtual void SetModelSpaceTransform(const glm::mat4& transform, int boneID) = 0;
	virtual std::vector<glm::mat4> GetModelSpaceTransofrms() const = 0;
};

}