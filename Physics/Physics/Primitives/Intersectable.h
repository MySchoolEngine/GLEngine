#pragma once

#include <Physics/Primitives/Intersection.h>

namespace GLEngine::Physics::Primitives {
template <class Derived> struct T_Intersectable {
public:
	[[nodiscard]] S_RayIntersection Intersect(const S_Ray& ray) const { return static_cast<const Derived*>(this)->IntersectImpl(ray); }
};
} // namespace GLEngine::Physics::Primitives
