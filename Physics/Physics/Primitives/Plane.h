#pragma once


#include <Physics/PhysicsApi.h>

#include <glm/glm.hpp>


namespace GLEngine {
namespace Physics {
namespace Primitives {
struct S_Ray;

struct PHYSICS_API_EXPORT S_Plane {
	glm::vec4	noraml;
	float		originOffset;
	float		Intersect(const S_Ray& ray) const;
};
}}}