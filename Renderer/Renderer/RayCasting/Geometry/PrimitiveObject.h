#pragma once

#include <Renderer/RayCasting/Geometry/GeometryTraits.h>
#include <Renderer/RayCasting/Geometry/SceneGeometry.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Sphere.h>
#include <Physics/Primitives/Triangle.h>

// The idea here is to lazy evaluate a lot of interesction calculations
// but e.g. triangle provides barycentric coordinates.
// Also, I would like to store additional info inside the triangle


namespace GLEngine::Renderer {

template <typename T> concept IntersectableConcept = requires(T t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection)
{
	{
		t.IntersectImpl(ray, intersection)
	}
	->std::convertible_to<float>;
}
|| requires(T t, const Physics::Primitives::S_Ray& ray)
{
	{
		t.IntersectImpl(ray)
	}
	->std::convertible_to<float>;
};

template <IntersectableConcept T> class C_Primitive : public I_RayGeometryObject {
public:
	C_Primitive(T primitive)
		: m_Primitive(primitive)
		, m_Area(RayTracing::T_GeometryTraits::GetArea(primitive))
	{
	}

	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const override
	{
		const auto t = m_Primitive.IntersectImpl(ray);
		if (t <= 0)
			return false;

		RayTracing::T_GeometryTraits::FillIntersection(m_Primitive, t, ray, intersection);
		intersection.SetMaterial(&GetMaterial());
		intersection.SetRayLength(t);
		return true;
	}
	[[nodiscard]] float Area() const override { return m_Area; }

	T m_Primitive;

private:
	float m_Area;
};
} // namespace GLEngine::Renderer