#include <RendererStdafx.h>

#include <Renderer/RayCasting/RayTraceScene.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>

namespace GLEngine::Renderer {

//=================================================================================
C_RayIntersection::C_RayIntersection(S_Frame&& frame, glm::vec3&& point, Physics::Primitives::S_Ray&& ray)
	: m_Frame(frame)
	, m_Point(point)
	, m_Ray(ray)
{
}

//=================================================================================
C_RayTraceScene::C_RayTraceScene()
{
	using namespace Physics::Primitives;
	auto							plane = std::make_unique<C_Primitive<S_Plane>>(S_Plane(glm::vec3(0, 1, 0), 0.0f));
	static const MeshData::Material mat1{glm::vec4{}, glm::vec4{255, 0, 255, 0}, glm::vec4{}, 1.f, 0};
	static const MeshData::Material mat2{glm::vec4{}, glm::vec4{0, 0, 255, 0}, glm::vec4{}, 1.f, 0};
	plane->SetMaterial(mat1);
	AddObejct(std::move(plane));

	static const auto normal2 = glm::vec3(0, 0, -1);
	auto			  disc	  = std::make_unique<C_Primitive<S_Disc>>(S_Disc{normal2, -normal2, 0.5f});
	disc->SetMaterial(mat2);
	AddObejct(std::move(disc));
}

//=================================================================================
C_RayTraceScene::~C_RayTraceScene() = default;

//=================================================================================
bool C_RayTraceScene::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, float offset) const
{
	std::vector<std::pair<C_RayIntersection, float>> intersections;

	std::for_each(m_Objects.begin(), m_Objects.end(), [&](const std::unique_ptr<I_RayGeometryObject>& object) {
		C_RayIntersection inter;
		if (object->Intersect(ray, inter))
		{
			if (inter.GetRayLength() >= offset)
				intersections.push_back({inter, inter.GetRayLength()});
		}
	});

	std::sort(intersections.begin(), intersections.end(), [](std::pair<C_RayIntersection, float>& a, std::pair<C_RayIntersection, float>& b) { return a.second < b.second; });

	if (intersections.empty())
		return false;

	intersection = intersections[0].first;
	return true;
}

//=================================================================================
void C_RayTraceScene::AddObejct(std::unique_ptr<I_RayGeometryObject>&& object)
{
	m_Objects.emplace_back(std::move(object));
}

} // namespace GLEngine::Renderer