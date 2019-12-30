#pragma once

#include <Physics/PhysicsApi.h>

#include <glm/glm.hpp>


namespace GLEngine::Physics::Primitives {
struct PHYSICS_API_EXPORT S_Ray {
	glm::vec4 origin;
	glm::vec4 direction;
};
}