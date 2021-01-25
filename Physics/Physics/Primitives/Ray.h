#pragma once

#include <glm/glm.hpp>


namespace GLEngine::Physics::Primitives {
struct S_Ray {
	glm::vec3 origin;
	glm::vec3 direction;
};
} // namespace GLEngine::Physics::Primitives