#include <RendererStdafx.h>

#include <Renderer/Colours.h>
#include <Renderer/Components/Lights/LightComponents.h>
#include <Renderer/Components/Lights/LightFunctions.h>
#include <Renderer/Components/Mesh/StaticMeshComponents.h>
#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/Lights/AreaLight.h>
#include <Renderer/Lights/PointLight.h>
#include <Renderer/Materials/MeshMaterialExtractor.h>
#include <Renderer/Mesh/Loading/MeshResource.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RayCasting/Geometry/PrimitiveObject.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/Geometry/RayTraceSceneBuilder.h>
#include <Renderer/RayCasting/Geometry/TrimeshModel.h>
#include <Renderer/RayCasting/Light/RayAreaLight.h>
#include <Renderer/RayCasting/Light/RayPointLight.h>

#include <Physics/Primitives/Rectangle.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/EntityManager.h>
#include <Entity/IEntity.h>
#include <Entity/World.h>

#include <Core/Resources/ResourceManager.h>

namespace GLEngine::Renderer::RayTracing {

namespace {

//=================================================================================
void ConvertMesh(const C_StaticMeshHandles& mesh, C_RayTraceScene& outScene)
{
	if (!mesh.IsMeshReady())
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "RayTraceSceneBuilder: mesh resource not ready, skipping component.");
		return;
	}

	auto triPath = mesh.GetMeshFile();
	triPath.replace_extension(".tri");

	const auto trimeshHandle = Core::C_ResourceManager::Instance().LoadResource<C_TrimeshModel>(triPath, /*isBlocking=*/true);
	if (!trimeshHandle.IsReady())
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "RayTraceSceneBuilder: could not build/load trimesh {}, skipping component.", triPath);
		return;
	}

	outScene.AddMesh(trimeshHandle, mesh.GetComponentModelMatrix(), mesh.GetMaterials());
}

//=================================================================================
void ConvertPointLight(const Renderer::C_PointLight& light, C_RayTraceScene& outScene)
{
	outScene.AddLight(std::make_shared<RayTracing::C_PointLight>(light.GetPosition(), light.GetColor() * light.GetIntensity()));
}

//=================================================================================
void ConvertAreaLight(const Renderer::C_AreaLight& light, C_RayTraceScene& outScene)
{
	static const MeshData::Material s_BlackAreaLightMaterial{.ambient			 = glm::vec3{},
															 .diffuse			 = glm::vec4{Colours::black, 0.f},
															 .specular			 = glm::vec3{},
															 .shininess			 = 0.f,
															 .textureIndex		 = -1,
															 .normalTextureIndex = -1,
															 .m_Name			 = "RayTraceSceneBuilder_AreaLight"};

	const auto normal = light.GetNormal();
	const auto up	  = light.GetUpVector();

	Physics::Primitives::S_Rectangle rect(Physics::Primitives::S_Plane(normal, glm::vec3(light.GetComponentModelMatrix()[3])), glm::cross(normal, up), up, light.GetWidth() / 2.f,
										  light.GetHeight() / 2.f);
	rect.plane.twoSided = false; // one-sided emitter, matches existing disc-light convention

	auto rectShape = std::make_shared<C_Primitive<Physics::Primitives::S_Rectangle>>(rect);
	rectShape->SetMaterial(outScene.AddMaterial(s_BlackAreaLightMaterial));

	outScene.AddLight(std::make_shared<RayTracing::C_AreaLight>(light.DiffuseColour(), rectShape));
}

//=================================================================================
void ConvertGraphicalComponent(const Entity::I_Entity& entity, const T_ComponentPtr& component, C_RayTraceScene& outScene)
{
	if (const auto mesh = std::dynamic_pointer_cast<C_StaticMeshHandles>(component))
	{
		ConvertMesh(*mesh, outScene);
		return;
	}
	CORE_LOG(E_Level::Warning, E_Context::Render, "RayTraceSceneBuilder: entity {} has an unsupported graphical component, skipping.", entity.GetName());
}

//=================================================================================
void ConvertLightComponent(const Entity::I_Entity& entity, const T_ComponentPtr& component, C_RayTraceScene& outScene)
{
	if (const auto pointLight = std::dynamic_pointer_cast<Renderer::C_PointLight>(component))
	{
		ConvertPointLight(*pointLight, outScene);
		return;
	}
	if (const auto areaLight = std::dynamic_pointer_cast<Renderer::C_AreaLight>(component))
	{
		ConvertAreaLight(*areaLight, outScene);
		return;
	}
	CORE_LOG(E_Level::Warning, E_Context::Render, "RayTraceSceneBuilder: entity {} has an unsupported light component, skipping.", entity.GetName());
}

//=================================================================================
void ConvertMesh(const S_StaticMesh& mesh, const Entity::S_WorldTransformComponent& worldTransform, C_RayTraceScene& outScene)
{
	if (!mesh.requested.IsReady())
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "RayTraceSceneBuilder: mesh resource not ready, skipping component.");
		return;
	}

	auto triPath = mesh.requested.GetResource().GetFilePath();
	triPath.replace_extension(".tri");

	const auto trimeshHandle = Core::C_ResourceManager::Instance().LoadResource<C_TrimeshModel>(triPath, /*isBlocking=*/true);
	if (!trimeshHandle.IsReady())
	{
		CORE_LOG(E_Level::Warning, E_Context::Render, "RayTraceSceneBuilder: could not build/load trimesh {}, skipping component.", triPath);
		return;
	}

	outScene.AddMesh(trimeshHandle, worldTransform.world, mesh.materials);
}

//=================================================================================
void ConvertPointLight(const S_PointLight& light, const Entity::S_WorldTransformComponent& worldTransform, C_RayTraceScene& outScene)
{
	outScene.AddLight(std::make_shared<RayTracing::C_PointLight>(glm::vec3(worldTransform.world[3]), light.color * light.intensity));
}

//=================================================================================
void ConvertAreaLight(const S_AreaLight& light, const Entity::S_WorldTransformComponent& worldTransform, C_RayTraceScene& outScene)
{
	static const MeshData::Material s_BlackAreaLightMaterial{.ambient			 = glm::vec3{},
															 .diffuse			 = glm::vec4{Colours::black, 0.f},
															 .specular			 = glm::vec3{},
															 .shininess			 = 0.f,
															 .textureIndex		 = -1,
															 .normalTextureIndex = -1,
															 .m_Name			 = "RayTraceSceneBuilder_AreaLight"};

	const auto normal = GetWorldNormal(worldTransform);
	const auto up	  = GetWorldUpVector(worldTransform);

	Physics::Primitives::S_Rectangle rect(Physics::Primitives::S_Plane(normal, glm::vec3(worldTransform.world[3])), glm::cross(normal, up), up, light.width / 2.f,
										  light.height / 2.f);
	rect.plane.twoSided = false; // one-sided emitter, matches existing disc-light convention

	auto rectShape = std::make_shared<C_Primitive<Physics::Primitives::S_Rectangle>>(rect);
	rectShape->SetMaterial(outScene.AddMaterial(s_BlackAreaLightMaterial));

	outScene.AddLight(std::make_shared<RayTracing::C_AreaLight>(light.diffuseColor, rectShape));
}

} // namespace

//=================================================================================
void BuildSceneFromEntityManager(const Entity::C_EntityManager& entityManager, C_RayTraceScene& outScene)
{
	for (const auto& entity : entityManager.GetEntities())
	{
		for (const auto& component : entity->GetComponents(Entity::E_ComponentType::Graphical))
		{
			ConvertGraphicalComponent(*entity, component, outScene);
		}
		for (const auto& component : entity->GetComponents(Entity::E_ComponentType::Light))
		{
			ConvertLightComponent(*entity, component, outScene);
		}
	}
	outScene.BuildScene();
}

//=================================================================================
void BuildSceneFromWorld(Entity::C_World& world, C_RayTraceScene& outScene)
{
	auto& registry = world.Registry();

	for (auto [e, mesh, worldTransform] : registry.view<S_StaticMesh, Entity::S_WorldTransformComponent>().each())
	{
		ConvertMesh(mesh, worldTransform, outScene);
	}
	for (auto [e, light, worldTransform] : registry.view<S_PointLight, Entity::S_WorldTransformComponent>().each())
	{
		ConvertPointLight(light, worldTransform, outScene);
	}
	for (auto [e, light, worldTransform] : registry.view<S_AreaLight, Entity::S_WorldTransformComponent>().each())
	{
		ConvertAreaLight(light, worldTransform, outScene);
	}
	outScene.BuildScene();
}

} // namespace GLEngine::Renderer::RayTracing
