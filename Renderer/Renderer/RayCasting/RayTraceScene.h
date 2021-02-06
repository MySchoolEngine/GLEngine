#pragma once

#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Frame.h>
#include <Renderer/RendererApi.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Shapes.h>

#include <utility>

namespace GLEngine::Renderer {

class RENDERER_API_EXPORT C_RayIntersection {
public:
	C_RayIntersection() = default;
	C_RayIntersection(S_Frame&& frame, glm::vec3&& point, Physics::Primitives::S_Ray&& ray);
	[[nodiscard]] const S_Frame&					GetFrame() const { return m_Frame; }
	[[nodiscard]] const Physics::Primitives::S_Ray& GetRay() const { return m_Ray; }
	[[nodiscard]] const glm::vec3&					GetIntersectionPoint() const { return m_Point; }
	[[nodiscard]] const float						GetRayLength() const { return glm::distance(m_Point, m_Ray.origin); }
	// material
	void									SetMaterial(const MeshData::Material* material) { m_Material = material; }
	[[nodiscard]] const MeshData::Material* GetMaterial() const { return m_Material; }

private:
	S_Frame					   m_Frame;
	glm::vec3				   m_Point;
	Physics::Primitives::S_Ray m_Ray;
	const MeshData::Material*  m_Material; // not owning
};

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
public:
	C_Primitive(T primitive)
		: m_Primitive(primitive)
		, m_Area(GetArea(primitive))
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
	[[nodiscard]] virtual float Area() const { return m_Area; }

private:
	static float GetArea(const Physics::Primitives::S_Plane&) { return std::numeric_limits<float>::infinity(); }
	static float GetArea(const Physics::Primitives::S_Disc& disc) { return disc.radius * disc.radius; }
	static float GetArea(const Physics::Primitives::S_Sphere&) { return std::numeric_limits<float>::infinity(); }

	void FillIntersection(const Physics::Primitives::S_Plane& plane, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
	{
		const auto normal = (glm::dot(plane.normal, -ray.direction) > 0 ? plane.normal : -plane.normal);
		intersection	  = C_RayIntersection(S_Frame(normal), ray.origin + ray.direction * t, Physics::Primitives::S_Ray(ray));
	}
	void FillIntersection(const Physics::Primitives::S_Disc& disc, float t, const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
	{
		FillIntersection(disc.plane, t, ray, intersection);
	}
	T	  m_Primitive;
	float m_Area;
};

class RENDERER_API_EXPORT C_RayTraceScene {
public:
	C_RayTraceScene();
	C_RayTraceScene(const C_RayTraceScene&) = delete;
	~C_RayTraceScene();

	void operator=(const C_RayTraceScene&) = delete;

	[[nodiscard]] bool Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, float offset = 0.f) const;
	void			   AddObejct(std::unique_ptr<I_RayGeometryObject>&& object);

private:
	std::vector<std::unique_ptr<I_RayGeometryObject>> m_Objects;
};

} // namespace GLEngine::Renderer