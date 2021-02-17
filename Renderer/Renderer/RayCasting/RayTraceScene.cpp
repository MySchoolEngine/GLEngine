#include <RendererStdafx.h>

#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/RayCasting/RayTraceScene.h>
#include <Renderer/RayCasting/SceneGeometry.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Shapes.h>

namespace GLEngine::Renderer {

//=================================================================================
C_RayTraceScene::C_RayTraceScene()
{
	using namespace Physics::Primitives;
	auto							plane  = std::make_shared<C_Primitive<S_Plane>>(S_Plane(glm::vec3(0, 1, 0), 0.0f));
	auto							plane1 = std::make_shared<C_Primitive<S_Plane>>(S_Plane(glm::vec3(1, 0, 0), {-3.f, 0.f, 0.f}));
	static const MeshData::Material mat1{glm::vec4{}, glm::vec4{255, 0, 255, 0}, glm::vec4{}, 1.f, 0};
	static const MeshData::Material mat2{glm::vec4{}, glm::vec4{0, 0, 255, 0}, glm::vec4{}, 1.f, 0};
	static const MeshData::Material mat3{glm::vec4{}, glm::vec4{36.f / 255.f, 36.f / 255.f, 36.f / 255.f, 0}, glm::vec4{}, 1.f, 0};
	plane->SetMaterial(mat1);
	AddObejct(std::move(plane));
	plane1->SetMaterial(mat3);
	AddObejct(std::move(plane1));

	static const auto normal2 = glm::vec3(0, 0, -1);
	auto			  sphere  = std::make_shared<C_Primitive<S_Sphere>>(S_Sphere{{0.f, 0.f, 0.f}, 1.f});
	sphere->SetMaterial(mat2);
	AddObejct(std::move(sphere));


	// lights
	const glm::vec3 lightNormal	  = glm::normalize(glm::vec3(0, -1.0, 1.0));
	auto			areaLightDisc = std::make_shared<C_Primitive<S_Disc>>(S_Disc(lightNormal, glm::vec3(0, 0, 0), 5.f));

	auto areaLight = std::make_shared<RayTracing::C_AreaLight>(glm::vec3(1.f, 1.f, 1.f), areaLightDisc);
	AddLight(std::move(areaLight));
}

//=================================================================================
C_RayTraceScene::~C_RayTraceScene() = default;

//=================================================================================
bool C_RayTraceScene::Intersect(const Physics::Primitives::S_Ray& ray, C_RayIntersection& intersection, float offset) const
{
	struct S_IntersectionInfo {
		C_RayIntersection					 intersection;
		float								 t;
		std::shared_ptr<I_RayGeometryObject> object;

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};
	std::vector<S_IntersectionInfo> intersections;

	std::for_each(m_Objects.begin(), m_Objects.end(), [&](const auto& object) {
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
	const auto it = std::find_if(m_AreaLights.begin(), m_AreaLights.end(),
								 [&](const std::shared_ptr<RayTracing::C_AreaLight>& other) { 
			return other->GetGeometry().get() == intersections[0].object.get(); 
		});
	if (it != m_AreaLights.end())
	{
		intersection.SetLight(*it);
	}
	return true;
}

//=================================================================================
void C_RayTraceScene::AddObejct(std::shared_ptr<I_RayGeometryObject>&& object)
{
	m_Objects.emplace_back(std::move(object));
}

//=================================================================================
void C_RayTraceScene::AddLight(std::shared_ptr<RayTracing::C_AreaLight>&& light)
{
	AddObejct(std::move(light->GetGeometry()));

	m_AreaLights.emplace_back(std::move(light));
}

} // namespace GLEngine::Renderer