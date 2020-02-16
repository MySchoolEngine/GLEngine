#pragma once

#include <Core/CoreMacros.h>

#include <Physics/Primitives/Ray.h>

#include <Core/GUID.h>

#include <glm/glm.hpp>


namespace GLEngine {
namespace Physics {
namespace Primitives {
struct PHYSICS_API_EXPORT S_RayIntersection {
	S_Ray		ray;
	GUID		entityId;

#pragma warning(push)
#pragma warning( disable : 4251)
	glm::vec4	intersectionPoint;
#pragma warning(pop)
	float		distance;
};
}
}
}