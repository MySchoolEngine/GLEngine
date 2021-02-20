#pragma once

#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Frame.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Shapes.h>
#include <Physics/Primitives/Triangle.h>

namespace GLEngine::Renderer {

class I_RayGeometryObject {
public:
	virtual ~I_RayGeometryObject()																						= default;
	[[nodiscard]] virtual bool	Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const = 0;
	[[nodiscard]] virtual float Area() const																			= 0;
	[[nodiscard]] float			Pdf() const { return 1.f / Area(); }

	void					  SetMaterial(const MeshData::Material& material) { m_Material = material; }
	const MeshData::Material& GetMaterial() const { return m_Material; }

private:
	MeshData::Material m_Material;
};

template <class T, typename = decltype(std::declval<T>().IntersectImpl(std::declval<const Physics::Primitives::S_Ray&>()))> class C_Primitive : public I_RayGeometryObject {
	using namespace Physics::Primitives;

public:
	C_Primitive(T primitive)
		: m_Primitive(primitive)
		, m_Area(GetArea(primitive))
	{
	}

	[[nodiscard]] virtual bool Intersect(const S_Ray& ray, C_RayIntersection& intersection) const override
	{
		const auto t = m_Primitive.IntersectImpl(ray);
		if (t <= 0)
			return false;

		FillIntersection(m_Primitive, t, ray, intersection);
		intersection.SetMaterial(&GetMaterial());
		return true;
	}
	[[nodiscard]] virtual float Area() const override { return m_Area; }

private:
	static float GetArea(const S_Plane&) { return std::numeric_limits<float>::infinity(); }
	static float GetArea(const S_Disc& disc) { return disc.radius * disc.radius; }
	static float GetArea(const S_Sphere&) { return std::numeric_limits<float>::infinity(); }
	static float GetArea(const S_Triangle& triangle) { return triangle.GetArea(); }

	void FillIntersection(const S_Plane& plane, float t, const S_Ray& ray, C_RayIntersection& intersection) const
	{
		const auto normal = (glm::dot(plane.normal, -ray.direction) > 0 ? plane.normal : -plane.normal);
		intersection	  = C_RayIntersection(S_Frame(normal), ray.origin + ray.direction * t, S_Ray(ray));
	}
	void FillIntersection(const S_Disc& disc, float t, const S_Ray& ray, C_RayIntersection& intersection) const { FillIntersection(disc.plane, t, ray, intersection); }
	void FillIntersection(const S_Triangle& triangle, float t, const S_Ray& ray, C_RayIntersection& intersection) const
	{
		intersection = C_RayIntersection(S_Frame(triangle.GetNormal()), ray.origin + ray.direction * t, S_Ray(ray));
	}
	void FillIntersection(const S_Sphere& sphere, float t, const S_Ray& ray, C_RayIntersection& intersection) const
	{
		const auto intersectionPoint = ray.origin + ray.direction * t;
		const auto normal			 = (intersectionPoint - sphere.m_position) / sphere.m_radius;
		intersection				 = C_RayIntersection(S_Frame(normal), glm::vec3(intersectionPoint), S_Ray(ray));
	}
	T	  m_Primitive;
	float m_Area;
};

} // namespace GLEngine::Renderer