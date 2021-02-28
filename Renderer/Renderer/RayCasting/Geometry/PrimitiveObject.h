#pragma once

#include <Renderer/RayCasting/Geometry/GeometryTraits.h>
#include <Renderer/RayCasting/Geometry/SceneGeometry.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Sphere.h>
#include <Physics/Primitives/Triangle.h>

namespace GLEngine::Renderer {

template <class T, typename = decltype(std::declval<T>().IntersectImpl(std::declval<const Physics::Primitives::S_Ray&>()))> class C_Primitive : public I_RayGeometryObject {
public:
	C_Primitive(T primitive)
		: m_Primitive(primitive)
		, m_Area(RayTracing::T_GeometryTraits::GetArea(primitive))
	{
	}

	[[nodiscard]] virtual bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const override
	{
		const auto t = m_Primitive.IntersectImpl(ray);
		if (t <= 0)
			return false;

		FillIntersection(m_Primitive, t, ray, intersection);
		intersection.SetMaterial(&GetMaterial());
		return true;
	}
	[[nodiscard]] virtual float Area() const override { return m_Area; }

	T m_Primitive;

private:
	void FillIntersection(const Physics::Primitives::S_Plane& plane, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
	{
		const auto normal = (glm::dot(plane.normal, -ray.direction) > 0 ? plane.normal : -plane.normal);
		intersection	  = C_RayIntersection(S_Frame(normal), ray.origin + ray.direction * t, Physics::Primitives::S_Ray(ray));
	}
	void FillIntersection(const Physics::Primitives::S_Disc& disc, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
	{
		FillIntersection(disc.plane, t, ray, intersection);
	}
	void FillIntersection(const Physics::Primitives::S_Triangle& triangle, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
	{
		intersection = C_RayIntersection(S_Frame(triangle.GetNormal()), ray.origin + ray.direction * t, Physics::Primitives::S_Ray(ray));
	}
	void FillIntersection(const Physics::Primitives::S_Sphere& sphere, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
	{
		const auto intersectionPoint = ray.origin + ray.direction * t;
		const auto normal			 = (intersectionPoint - sphere.m_position) / sphere.m_radius;
		intersection				 = C_RayIntersection(S_Frame(normal), glm::vec3(intersectionPoint), Physics::Primitives::S_Ray(ray));
	}
	float m_Area;
};
} // namespace GLEngine::Renderer