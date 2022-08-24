#include <RendererStdafx.h>

#include <Renderer/RayCasting/Geometry/GeometryList.h>
#include <Renderer/RayCasting/RayIntersection.h>

namespace GLEngine::Renderer {

//=================================================================================
bool C_GeometryList::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
{
	if (m_AABB.IntersectImpl(ray) <= 0.f)
		return false;
	struct S_IntersectionInfo {
		C_RayIntersection					 intersection;
		float								 t;
		std::shared_ptr<I_RayGeometryObject> object;

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};
	S_IntersectionInfo closestIntersect{C_RayIntersection(), std::numeric_limits<float>::max()};


	std::for_each(m_Geometry.begin(), m_Geometry.end(), [&](const auto& object) {
		constexpr auto	  offset = 1e-3f;
		C_RayIntersection inter;
		if (object->Intersect(ray, inter))
		{
			if (inter.GetRayLength() >= offset && inter.GetRayLength() < closestIntersect.t)
				closestIntersect = {inter, inter.GetRayLength(), object};
		}
	});

	if (closestIntersect.t == std::numeric_limits<float>::max())
		return false;


	intersection = closestIntersect.intersection;
	return true;
}

//=================================================================================
void C_GeometryList::AddObject(std::shared_ptr<I_RayGeometryObject>&& object)
{
	m_Geometry.emplace_back(object);
}

} // namespace GLEngine::Renderer