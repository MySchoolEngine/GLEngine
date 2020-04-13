#pragma once

#include <glm/glm.hpp>


namespace GLEngine::Physics::Primitives {
struct S_Ray {
	glm::vec4 origin;
	glm::vec4 direction;
};
}