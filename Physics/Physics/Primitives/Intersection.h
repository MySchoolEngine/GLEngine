#pragma once

#include <Core/CoreMacros.h>

#include <Physics/Primitives/Ray.h>

#include <Core/GUID.h>

#include <glm/glm.hpp>


namespace GLEngine {
namespace Physics {
namespace Primitives {
struct API_EXPORT S_RayIntersection {
	S_Ray		ray;
	GUID		entityId;
	glm::vec4	intersectionPoint;
	float		distance;
};
}
}
}