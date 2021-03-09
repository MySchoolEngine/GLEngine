#include <RendererStdafx.h>

#include <Renderer/RayCasting/Geometry/GeometryList.h>
#include <Renderer/RayCasting/RayIntersection.h>

namespace GLEngine::Renderer {

//=================================================================================
bool C_GeometryList::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection) const
{
	if (m_AABB.IntersectImpl(ray) <= 0.f)
		return false;
	auto offset = 1e-3f;
	struct S_IntersectionInfo {
		C_RayIntersection					 intersection;
		float								 t;
		std::shared_ptr<I_RayGeometryObject> object;

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};
	std::vector<S_IntersectionInfo> intersections;
	intersections.reserve(5);


	std::for_each(m_Geometry.begin(), m_Geometry.end(), [&](const auto& object) {
		C_RayIntersection inter;
		if (object->Intersect(ray, inter))
		{
			if (inter.GetRayLength() >= offset)
				intersections.push_back({inter, inter.GetRayLength(), object});
		}
	});


	std::sort(intersections.begin(), intersections.end());

	if (intersections.empty())
		return false;


	intersection = intersections[0].intersection;
	return true;
}

//=================================================================================
void C_GeometryList::AddObject(std::shared_ptr<I_RayGeometryObject>&& object)
{
	m_Geometry.emplace_back(object);
}

} // namespace GLEngine::Renderer