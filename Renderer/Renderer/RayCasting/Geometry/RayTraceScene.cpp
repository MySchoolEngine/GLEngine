#include <RendererStdafx.h>

#include <Renderer/Colours.h>
#include <Renderer/Mesh/Loading/ModelLoader.h>
#include <Renderer/RayCasting/Geometry/GeometryList.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/Geometry/SceneGeometry.h>
#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/RayCasting/Light/RayPointLight.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/Textures/TextureLoader.h>
#include <Renderer/Textures/TextureStorage.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Sphere.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Renderer {
#define CORNELL
//=================================================================================
C_RayTraceScene::C_RayTraceScene()
{
	using namespace Physics::Primitives;
#ifdef CORNELL
	Textures::TextureLoader tl;
	m_Textures.emplace_back(tl.loadTexture(R"(Models\Bricks01\REGULAR\1K\Bricks01_COL_VAR2_1K.bmp)"));

	static const MeshData::Material red{glm::vec4{},		glm::vec4{Colours::red, 0},		glm::vec4{}, 0.f, -1};
	static const MeshData::Material green{glm::vec4{},		glm::vec4{Colours::green, 0},	glm::vec4{}, 0.f, -1};
	static const MeshData::Material white{glm::vec4{},		glm::vec4{Colours::white, 0},	glm::vec4{}, 0.f, -1};
	static const MeshData::Material brick{glm::vec4{},		glm::vec4{Colours::white, 0},	glm::vec4{}, 0.f,  0}; // brick texture
	static const MeshData::Material blue{glm::vec4{},		glm::vec4{Colours::blue, 0},	glm::vec4{}, 0.f, -1};
	static const MeshData::Material blueMirror{glm::vec4{},	glm::vec4{Colours::blue, 0},	glm::vec4{}, 1.f, -1};
	static const MeshData::Material black{glm::vec4{},		glm::vec4{Colours::black, 0.f}, glm::vec4{}, 0.f, -1};

	{
		auto trimesh = std::make_shared<C_Trimesh>();
		// floor
		auto triangle  = S_Triangle({-3.f, -1.5f, 3.f}, {3.f, -1.5f, -3.f}, {-3.f, -1.5f, -3.f});
		auto triangle1 = S_Triangle({-3.f, -1.5f, 3.f}, {3.f, -1.5f, 3.f}, {3.f, -1.5f, -3.f});
		trimesh->AddTriangle(triangle, {glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f)});
		trimesh->AddTriangle(triangle1, {glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
		trimesh->SetMaterial(brick);

		AddObejct(trimesh);
	}

	{
		// ceiling
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, 1.5f, 3.f}, {-3.f, 1.5f, -3.f}, {3.f, 1.5f, -3.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, 1.5f, 3.f}, {3.f, 1.5f, -3.f}, {3.f, 1.5f, 3.f}));
		triangle->SetMaterial(white);
		triangle1->SetMaterial(white);
		AddObejct(triangle);
		AddObejct(triangle1);
	}

	{
		// left wall - red
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, -1.5f, 3.f}, {-3.f, -1.5f, -3.f}, {-3.f, 1.5f, -3.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, -1.5f, 3.f}, {-3.f, 1.5f, -3.f}, {-3.f, 1.5f, 3.f}));
		triangle->SetMaterial(red);
		triangle1->SetMaterial(red);
		AddObejct(triangle);
		AddObejct(triangle1);
	}

	{
		// left wall - green
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({3.f, -1.5f, 3.f}, {3.f, 1.5f, -3.f}, {3.f, -1.5f, -3.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({3.f, -1.5f, 3.f}, {3.f, 1.5f, 3.f}, {3.f, 1.5f, -3.f}));
		triangle->SetMaterial(green);
		triangle1->SetMaterial(green);
		AddObejct(triangle);
		AddObejct(triangle1);
	}

	{
		// back wall
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, -1.5f, -3.f}, {3.f, -1.5f, -3.f}, {3.f, 1.5f, -3.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, -1.5f, -3.f}, {3.f, 1.5f, -3.f}, {-3.f, 1.5f, -3.f}));
		triangle->SetMaterial(white);
		triangle1->SetMaterial(white);
		AddObejct(triangle);
		AddObejct(triangle1);
	}

	{
		// sphere
		auto sphere = std::make_shared<C_Primitive<S_Sphere>>(S_Sphere{{-1.5f, -1.f, -1.5f}, 1.f});
		sphere->SetMaterial(blueMirror);
		AddObejct(std::move(sphere));
	}

	{
		// sphere
		auto sphere = std::make_shared<C_Primitive<S_Sphere>>(S_Sphere{{.8f, 0.f, .5f}, 1.f});
		sphere->SetMaterial(blue);
		AddObejct(std::move(sphere));
	}

	constexpr bool discLight = false;
	if (discLight)
	{
		// light
		const glm::vec3 lightNormal = glm::normalize(glm::vec3(0, -1.0, 0));
		auto			disc		= S_Disc(lightNormal, glm::vec3(0, 1.43f, 0), 0.7f);
		disc.plane.twoSided			= false;
		auto areaLightDisc			= std::make_shared<C_Primitive<S_Disc>>(disc);
		areaLightDisc->SetMaterial(black);

		auto areaLight = std::make_shared<RayTracing::C_AreaLight>(5.f * glm::vec3(1.f, 1.f, .3f), areaLightDisc);
		AddLight(std::move(areaLight));
	}
	else
	{
		// ceiling
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-1.f, 1.5f - .01f, 1.f}, {-1.f, 1.5f - .01f, -1.f}, {1.f, 1.5f - .01f, -1.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-1.f, 1.5f - .01f, 1.f}, {1.f, 1.5f - .01f, -1.f}, {1.f, 1.5f - .01f, 1.f}));
		triangle->SetMaterial(white);
		triangle1->SetMaterial(white);
		auto areaLight = std::make_shared<RayTracing::C_AreaLight>(5.f * glm::vec3(1.f, 1.f, .3f) * 0.5f, triangle);
		AddLight(std::move(areaLight));
		auto areaLight1 = std::make_shared<RayTracing::C_AreaLight>(5.f * glm::vec3(1.f, 1.f, .3f) * 0.5f, triangle1);
		AddLight(std::move(areaLight1));
	}

	if (false)
	{
		// model
		auto					 scene = std::make_shared<MeshData::Scene>();
		std::vector<std::string> textures;
		Mesh::ModelLoader		 ml;
		ml.Reset();
		if (ml.addModelFromFileToScene("Models/sword/baphomet-sword-mostruario.obj", scene, textures))
			AddMesh(scene->meshes[0]);
	}
#else
	auto							plane = std::make_shared<C_Primitive<S_Plane>>(S_Plane(glm::vec3(1, 0, 0), {-3.f, 0.f, 0.f}));
	static const MeshData::Material mat1{glm::vec4{}, glm::vec4{Colours::purple, 0}, glm::vec4{}, 1.f, -1};
	static const MeshData::Material mat2{glm::vec4{}, glm::vec4{Colours::blue, 0}, glm::vec4{}, 1.f, -1};
	static const MeshData::Material mat3{glm::vec4{}, glm::vec4{36.f / 255.f, 36.f / 255.f, 36.f / 255.f, 0}, glm::vec4{}, 1.f, -1};

	auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, 0.f, 3.f}, {3.f, 0.f, -3.f}, {-3.f, 0.f, -3.f}));
	auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, 0.f, 3.f}, {3.f, 0.f, 3.f}, {3.f, 0.f, -3.f}));
	triangle->SetMaterial(mat1);
	AddObejct(triangle);
	triangle->SetMaterial(mat1);
	AddObejct(triangle1);

	// plane->SetMaterial(mat3);
	// AddObejct(std::move(plane));

	static const auto normal2 = glm::vec3(0, 0, -1);
	auto			  sphere  = std::make_shared<C_Primitive<S_Sphere>>(S_Sphere{{0.f, 0.f, 0.f}, 1.f});
	sphere->SetMaterial(mat2);
	AddObejct(std::move(sphere));

	// lights
	const glm::vec3 lightNormal = glm::normalize(glm::vec3(0, -1.0, 1.0));
	auto			disc		= S_Disc(lightNormal, glm::vec3(0, 0, 0), 2.5f);
	disc.plane.twoSided			= false;
	auto areaLightDisc			= std::make_shared<C_Primitive<S_Disc>>(disc);

	auto areaLight = std::make_shared<RayTracing::C_AreaLight>(glm::vec3(1.f, 1.f, 1.f), areaLightDisc);
	AddLight(std::move(areaLight));
	AddLight(std::make_shared<RayTracing::C_PointLight>(glm::vec3(0, 1, 0), glm::vec3(0.4, 0.3, 0.1)));
	AddLight(std::make_shared<RayTracing::C_PointLight>(glm::vec3(3, 1, 0), glm::vec3(0, 0.6, 0.1)));
	AddLight(std::make_shared<RayTracing::C_PointLight>(glm::vec3(3, 3, -5), glm::vec3(1.f, 1.f, 1.f)));
#endif
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
	intersections.reserve(5);

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

	intersection  = intersections[0].intersection;
	const auto it = std::find_if(m_AreaLights.begin(), m_AreaLights.end(),
								 [&](const std::shared_ptr<RayTracing::C_AreaLight>& other) { return other->GetGeometry().get() == intersections[0].object.get(); });
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

//=================================================================================
void C_RayTraceScene::AddLight(std::shared_ptr<RayTracing::C_PointLight>&& light)
{
	m_PointLights.emplace_back(std::move(light));
}

//=================================================================================
void C_RayTraceScene::ForEachLight(std::function<void(const std::reference_wrapper<const RayTracing::I_RayLight>& light)> fnc) const
{
	std::for_each(m_AreaLights.begin(), m_AreaLights.end(), [&](const std::shared_ptr<RayTracing::C_AreaLight>& light) { fnc(*(light.get())); });
	std::for_each(m_PointLights.begin(), m_PointLights.end(), [&](const std::shared_ptr<RayTracing::C_PointLight>& light) { fnc(*(light.get())); });
}

//=================================================================================
void C_RayTraceScene::AddMesh(const MeshData::Mesh& mesh)
{
	Utils::HighResolutionTimer renderTime;
	using namespace Physics::Primitives;
	static const MeshData::Material blue{glm::vec4{}, glm::vec4{0, 0, 255, 0}, glm::vec4{}, 1.f, 0};
//#define OLD_TRIMESH
#ifdef OLD_TRIMESH
	auto list = std::make_shared<C_GeometryList>();
	for (auto it = mesh.vertices.begin(); it != mesh.vertices.end(); it += 3)
	{
		auto triangle = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle(glm::vec3(*it), glm::vec3(*(it + 1)), glm::vec3(*(it + 2))));
		triangle->SetMaterial(blue);
		list->AddObject(triangle);
	}

	list->GetAABB() = mesh.bbox;
	AddObejct(list);
#else
	auto trimesh = std::make_shared<C_Trimesh>();
	trimesh->SetMaterial(blue);
	trimesh->AddMesh(mesh);
	AddObejct(trimesh);
#endif
	CORE_LOG(E_Level::Warning, E_Context::Render, "Ray trace: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
}

//=================================================================================
const C_TextureView C_RayTraceScene::GetTextureView(int textureID) const
{
	return C_TextureView(m_Textures[textureID].get());
}

} // namespace GLEngine::Renderer