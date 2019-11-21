#pragma once

#include <Renderer/Animation/Joint.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace GLEngine::Renderer::Animation{

class API_EXPORT C_Skeleton{
public:
	C_Skeleton() = default;

	void InitializeSkeleton();
public:
	std::unique_ptr<S_Joint> m_Root;
};

}