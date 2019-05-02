#pragma once

#include <Core/CoreMacros.h>

#include <glm/glm.hpp>


namespace GLEngine {
namespace Physics {
namespace Primitives {
struct API_EXPORT S_Ray {
	glm::vec4 origin;
	glm::vec4 direction;
};
}}}