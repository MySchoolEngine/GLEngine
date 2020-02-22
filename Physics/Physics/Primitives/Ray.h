#pragma once

#include <Physics/PhysicsApi.h>

#include <glm/glm.hpp>


namespace GLEngine::Physics::Primitives {
struct PHYSICS_API_EXPORT S_Ray {
#pragma warning(push)
#pragma warning( disable : 4251)
	glm::vec4 origin;
	glm::vec4 direction;
#pragma warning(pop)
};
}