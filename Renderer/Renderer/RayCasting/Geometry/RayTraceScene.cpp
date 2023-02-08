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
#include <Renderer/RayCasting/Material/DiffuseMaterial.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/Textures/TextureResource.h>
#include <Renderer/Textures/TextureStorage.h>

#include <Physics/Primitives/Disc.h>
#include <Physics/Primitives/Plane.h>
#include <Physics/Primitives/Sphere.h>

#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/HighResolutionTimer.h>

namespace GLEngine::Renderer {
#define CORNELL
//=================================================================================
C_RayTraceScene::C_RayTraceScene()
{
	using namespace Physics::Primitives;
#ifdef CORNELL
	auto& rm = Core::C_ResourceManager::Instance();
	m_Textures.emplace_back(rm.LoadResource<TextureResource>(std::filesystem::path(R"(Models\Bricks01\REGULAR\1K\Bricks01_COL_VAR2_1K.bmp)")));
	m_LoadingTextures.AddHandle(m_Textures[0]);

	static const MeshData::Material red{glm::vec4{}, glm::vec4{Colours::red, 0}, glm::vec4{}, 0.f, -1, -1, "red"};
	static const MeshData::Material green{glm::vec4{}, glm::vec4{Colours::green, 0}, glm::vec4{}, 0.f, -1, -1, "green"};
	static const MeshData::Material white{glm::vec4{}, glm::vec4{Colours::white, 0}, glm::vec4{}, 0.f, -1, -1, "white"};
	static const MeshData::Material brick{glm::vec4{}, glm::vec4{Colours::white, 0}, glm::vec4{}, 0.f, 0, -1, "brick"}; // brick texture
	static const MeshData::Material blue{glm::vec4{}, glm::vec4{Colours::blue, 0}, glm::vec4{}, 0.f, -1, -1, "blue"};
	static const MeshData::Material blueMirror{glm::vec4{}, glm::vec4{Colours::blue, 0}, glm::vec4{}, 1.f, -1, -1, "blueMirror"};
	static const MeshData::Material black{glm::vec4{}, glm::vec4{Colours::black, 0.f}, glm::vec4{}, 0.f, -1, -1, "black"};

	auto* redMat		= AddMaterial(red).get();
	auto* greenMat		= AddMaterial(green).get();
	auto* whiteMat		= AddMaterial(white).get();
	auto* brickMat		= AddMaterial(brick).get();
	auto* blueMat		= AddMaterial(blue).get();
	auto* blueMirrorMat = AddMaterial(blueMirror).get();
	auto* blackMat		= AddMaterial(black).get();

	{
		auto trimesh = std::make_shared<C_Trimesh>();
		// floor
		auto triangle  = S_Triangle({-3.f, -1.5f, 3.f}, {3.f, -1.5f, -3.f}, {-3.f, -1.5f, -3.f});
		auto triangle1 = S_Triangle({-3.f, -1.5f, 3.f}, {3.f, -1.5f, 3.f}, {3.f, -1.5f, -3.f});
		trimesh->AddTriangle(triangle, {glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 1.0f)});
		trimesh->AddTriangle(triangle1, {glm::vec2(1.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 1.0f)});
		trimesh->SetMaterial(brickMat);

		AddObejct(trimesh);
	}

	{
		// ceiling
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, 1.5f, 3.f}, {-3.f, 1.5f, -3.f}, {3.f, 1.5f, -3.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, 1.5f, 3.f}, {3.f, 1.5f, -3.f}, {3.f, 1.5f, 3.f}));
		triangle->SetMaterial(whiteMat);
		triangle1->SetMaterial(whiteMat);
		AddObejct(triangle);
		AddObejct(triangle1);
	}

	{
		// left wall - red
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, -1.5f, 3.f}, {-3.f, -1.5f, -3.f}, {-3.f, 1.5f, -3.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, -1.5f, 3.f}, {-3.f, 1.5f, -3.f}, {-3.f, 1.5f, 3.f}));
		triangle->SetMaterial(redMat);
		triangle1->SetMaterial(redMat);
		AddObejct(triangle);
		AddObejct(triangle1);
	}

	{
		// left wall - green
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({3.f, -1.5f, 3.f}, {3.f, 1.5f, -3.f}, {3.f, -1.5f, -3.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({3.f, -1.5f, 3.f}, {3.f, 1.5f, 3.f}, {3.f, 1.5f, -3.f}));
		triangle->SetMaterial(greenMat);
		triangle1->SetMaterial(greenMat);
		AddObejct(triangle);
		AddObejct(triangle1);
	}

	{
		// back wall
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, -1.5f, -3.f}, {3.f, -1.5f, -3.f}, {3.f, 1.5f, -3.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-3.f, -1.5f, -3.f}, {3.f, 1.5f, -3.f}, {-3.f, 1.5f, -3.f}));
		triangle->SetMaterial(whiteMat);
		triangle1->SetMaterial(whiteMat);
		AddObejct(triangle);
		AddObejct(triangle1);
	}

	if (false)
	{
		// sphere
		auto sphere = std::make_shared<C_Primitive<S_Sphere>>(S_Sphere{{-1.5f, -1.f, -1.5f}, 1.f});
		sphere->SetMaterial(blueMirrorMat);
		AddObejct(std::move(sphere));
	}

	if (false)
	{
		// sphere
		auto sphere = std::make_shared<C_Primitive<S_Sphere>>(S_Sphere{{.8f, 0.f, .5f}, 1.f});
		sphere->SetMaterial(blueMat);
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
		areaLightDisc->SetMaterial(blackMat);

		auto areaLight = std::make_shared<RayTracing::C_AreaLight>(5.f * glm::vec3(1.f, 1.f, .3f), areaLightDisc);
		AddLight(std::move(areaLight));
	}
	else
	{
		// ceiling
		auto triangle  = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-1.f, 1.5f - .01f, 1.f}, {-1.f, 1.5f - .01f, -1.f}, {1.f, 1.5f - .01f, -1.f}));
		auto triangle1 = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle({-1.f, 1.5f - .01f, 1.f}, {1.f, 1.5f - .01f, -1.f}, {1.f, 1.5f - .01f, 1.f}));
		triangle->SetMaterial(whiteMat);
		triangle1->SetMaterial(whiteMat);
		auto areaLight = std::make_shared<RayTracing::C_AreaLight>(5.f * glm::vec3(1.f, 1.f, .3f) * 0.5f, triangle);
		AddLight(std::move(areaLight));
		auto areaLight1 = std::make_shared<RayTracing::C_AreaLight>(5.f * glm::vec3(1.f, 1.f, .3f) * 0.5f, triangle1);
		AddLight(std::move(areaLight1));
	}

	if (false)
	{
		auto& meshHandle = m_Meshes.emplace_back(rm.LoadResource<MeshResource>(R"(Models/sword/baphomet-sword-mostruario.obj)"));
		m_LoadingMeshes.AddHandle(meshHandle);
	}

	if (true)
	{
		auto& meshHandle = m_Meshes.emplace_back(rm.LoadResource<MeshResource>(R"(Models/KoboldRig/Kobold_Rig_.obj)"));
		m_LoadingMeshes.AddHandle(meshHandle);
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
		C_RayIntersection	 intersection;
		float				 t;
		I_RayGeometryObject* object;

		[[nodiscard]] bool operator<(const S_IntersectionInfo& a) const { return t < a.t; }
	};
	S_IntersectionInfo closestIntersect{C_RayIntersection(), std::numeric_limits<float>::max(), nullptr};

	std::for_each(m_Objects.begin(), m_Objects.end(), [&](const auto& object) {
		C_RayIntersection inter;
		if (object->Intersect(ray, inter))
		{
			if (inter.GetRayLength() >= offset && inter.GetRayLength() < closestIntersect.t)
				closestIntersect = {inter, inter.GetRayLength(), object.get()};
		}
	});

	if (closestIntersect.t == std::numeric_limits<float>::max())
		return false;

	intersection  = closestIntersect.intersection;
	const auto it = std::find_if(m_AreaLights.begin(), m_AreaLights.end(),
								 [&](const std::shared_ptr<RayTracing::C_AreaLight>& other) { return other->GetGeometry().get() == closestIntersect.object; });
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
void C_RayTraceScene::AddMesh(const MeshData::Mesh& mesh, const MeshData::Material& material)
{
	Utils::HighResolutionTimer renderTime;
	using namespace Physics::Primitives;
//#define OLD_TRIMESH
#ifdef OLD_TRIMESH
	auto list = std::make_shared<C_GeometryList>();
	for (auto it = mesh.vertices.begin(); it != mesh.vertices.end(); it += 3)
	{
		auto triangle = std::make_shared<C_Primitive<S_Triangle>>(S_Triangle(glm::vec3(*it), glm::vec3(*(it + 1)), glm::vec3(*(it + 2))));
		triangle->SetMaterial(material);
		list->AddObject(triangle);
	}

	list->GetAABB() = mesh.bbox;
	AddObejct(list);
#else
	auto trimesh = std::make_shared<C_Trimesh>();
	trimesh->SetMaterial(AddMaterial(material).get());
	trimesh->AddMesh(mesh);
	trimesh->SetTransformation(glm::translate(glm::mat4(1.f), glm::vec3(0, -1.5f, 0)) * glm::scale(glm::mat4(1.f), glm::vec3(0.5f, 0.5f, 0.5f)));
	m_Trimeshes.push_back(trimesh);
	AddObejct(trimesh);
#endif
	CORE_LOG(E_Level::Info, E_Context::Render, "Raytracing add mesh: {}ms", renderTime.getElapsedTimeFromLastQueryMilliseconds());
}

//=================================================================================
const C_TextureView C_RayTraceScene::GetTextureView(int textureID) const
{
	// because the truly const texture view is not implemented I need const cast here
	return C_TextureView(const_cast<I_TextureViewStorage*>(&(m_Textures[textureID].GetResource().GetStorage())));
}

//=================================================================================
void C_RayTraceScene::DebugDraw(I_DebugDraw* dd) const
{
	std::for_each(m_Trimeshes.begin(), m_Trimeshes.end(), [&](const auto& trimesh) { trimesh->DebugDraw(dd); });
}

//=================================================================================
bool C_RayTraceScene::IsLoaded() const
{
	return m_LoadingMeshes.IsDone() && m_LoadingTextures.IsDone();
}

//=================================================================================
void C_RayTraceScene::BuildScene()
{
	auto& rm = Core::C_ResourceManager::Instance();
	if (m_LoadingMeshes.IsDone())
	{
		for (const auto& meshHandle : m_Meshes)
		{
			GLE_ASSERT(meshHandle.IsLoading() == false, "At this point all the handles should be loaded!");
			if (!meshHandle)
				continue;

			for (auto& mesh : meshHandle.GetResource().GetScene().meshes)
			{
				// intentional copy as I need to re-number the textures
				m_LoadingTextures.AddHandle(m_Textures[0]);
				const auto&		   scene = meshHandle.GetResource().GetScene();
				MeshData::Material mat	 = scene.materials[mesh.materialIndex];

				if (mat.textureIndex != -1)
				{
					m_Textures.emplace_back(rm.LoadResource<TextureResource>(std::filesystem::path(meshHandle.GetResource().GetTextureNames()[mat.textureIndex])));
					mat.textureIndex = m_Textures.size() - 1;
				}
				if (mat.noramlTextureIndex != -1)
				{
					m_Textures.emplace_back(rm.LoadResource<TextureResource>(std::filesystem::path(meshHandle.GetResource().GetTextureNames()[mat.textureIndex])));
					mat.noramlTextureIndex = m_Textures.size() - 1;
				}
				mat.shininess		   = 0.f;
				AddMesh(mesh, mat);
			}
		}
	}
}

//=================================================================================
std::unique_ptr<I_MaterialInterface>& C_RayTraceScene::AddMaterial(const MeshData::Material& material)
{
	if (material.shininess == 0.f)
	{
		Core::ResourceHandle<TextureResource> texture = {};
		if (material.textureIndex != -1) 
		{
			texture = m_Textures[material.textureIndex];
		}
		return m_Materials.emplace_back(std::make_unique<C_DiffuseMaterial>(material.diffuse, texture));
	}
	else
	{
		// todo glossy mat
		return m_Materials.emplace_back(std::make_unique<C_DiffuseMaterial>(material.diffuse));
	}
}

} // namespace GLEngine::Renderer