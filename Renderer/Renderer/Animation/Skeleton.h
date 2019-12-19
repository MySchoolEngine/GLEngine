#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Animation/Joint.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GLEngine::Renderer::Animation{

class RENDERER_API_EXPORT C_Skeleton{
public:
	C_Skeleton() = default;
public:
	std::unique_ptr<S_Joint> m_Root;
};

}