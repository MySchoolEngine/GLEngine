#pragma once

#include <Physics/Primitives/Ray.h>

#include <Core/GUID.h>


namespace GLEngine::Physics::Primitives {
struct S_RayIntersection {
	S_Ray ray;
	GUID  entityId;

	glm::vec3 intersectionPoint;
	float	  distance;
};
} // namespace GLEngine::Physics::Primitives