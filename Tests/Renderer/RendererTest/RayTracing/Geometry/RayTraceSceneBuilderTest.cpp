#include <RendererTestStdafx.h>

#include <Renderer/Colours.h>
#include <Renderer/Components/Lights/LightComponents.h>
#include <Renderer/Components/Mesh/StaticMeshComponents.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/Geometry/RayTraceSceneBuilder.h>
#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/VisibilityTester.h>

#include <Physics/Primitives/Frustum.h>
#include <Physics/Primitives/Ray.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

#include <glm/gtx/transform.hpp>

namespace GLEngine::Renderer {

// ============================================================================
// RayTraceSceneBuilder::BuildSceneFromWorld
// ============================================================================

class RayTraceSceneBuilderFromWorldFixture : public ::testing::Test {
protected:
	Entity::C_World world;
	C_RayTraceScene scene;
};

TEST_F(RayTraceSceneBuilderFromWorldFixture, MeshComponentWithoutReadyResourceIsSkippedWithoutCrashing)
{
	auto entity = world.CreateEntity("Mesh");
	entity.Add<S_StaticMesh>(); // requested left default-constructed: never ready

	RayTracing::BuildSceneFromWorld(world, scene);

	int lightCount = 0;
	scene.ForEachLight([&](const auto&) { ++lightCount; });
	EXPECT_EQ(lightCount, 0);
}

TEST_F(RayTraceSceneBuilderFromWorldFixture, PointLightIsConvertedWithWorldPositionAndIntensity_ThroughHierarchy)
{
	auto parent											   = world.CreateEntity("Parent");
	auto light											   = world.CreateEntity("PointLight");
	parent.Get<Entity::S_TransformComponent>().translation = glm::vec3(1, 2, 3);
	ASSERT_TRUE(world.SetParent(light.Handle(), parent.Handle()));
	light.Add<S_PointLight>(); // default intensity/color, world position comes entirely from the parent

	Entity::TransformSystem::Update(world);
	RayTracing::BuildSceneFromWorld(world, scene);

	int deltaLightCount = 0;
	scene.ForEachLight([&](const std::reference_wrapper<const RayTracing::I_RayLight>& l) {
		if (l.get().IsDeltaLight())
			++deltaLightCount;
	});
	EXPECT_EQ(deltaLightCount, 1);

	// SampleLi from a point right above the light's position should report a positive radiance,
	// confirming the light was placed at the parent's world translation (1,2,3).
	C_RayIntersection hit(S_Frame(glm::vec3(0, -1, 0)), glm::vec3(1, 3, 3), Physics::Primitives::S_Ray(glm::vec3(1, 3, 3), glm::vec3(0, -1, 0)));
	C_STDSampler	  sampler(0.f, 1.f);
	scene.ForEachLight([&](const std::reference_wrapper<const RayTracing::I_RayLight>& l) {
		if (!l.get().IsDeltaLight())
			return;
		RayTracing::S_VisibilityTester vis(glm::vec3(0.f), glm::vec3(0.f));
		float						   pdf = 0.f;
		const auto					   Li  = l.get().SampleLi(hit, sampler, vis, &pdf);
		EXPECT_GT(Li.x + Li.y + Li.z, 0.f);
	});
}

TEST_F(RayTraceSceneBuilderFromWorldFixture, AreaLightGeometryIsIntersectableAtItsWorldTransform_ThroughHierarchy)
{
	auto parent											   = world.CreateEntity("Parent");
	auto light											   = world.CreateEntity("AreaLight");
	parent.Get<Entity::S_TransformComponent>().translation = glm::vec3(0, 5, 0);
	ASSERT_TRUE(world.SetParent(light.Handle(), parent.Handle()));
	light.Add<S_AreaLight>(); // default width/height/diffuseColor, world position comes from the parent

	Entity::TransformSystem::Update(world);
	RayTracing::BuildSceneFromWorld(world, scene);

	int areaLightCount = 0;
	scene.ForEachLight([&](const std::reference_wrapper<const RayTracing::I_RayLight>& l) {
		if (!l.get().IsDeltaLight())
			++areaLightCount;
	});
	ASSERT_EQ(areaLightCount, 1);

	// Same convention as the entity-manager-based test above: default normal/up face -Z/+Y, so the
	// rectangle sits at the parent's translation with normal (0,0,-1); approach from -Z heading +Z.
	constexpr Physics::Primitives::S_Ray ray(glm::vec3(0, 5, -5), glm::vec3(0, 0, 1));
	C_RayIntersection					 intersection;
	ASSERT_TRUE(scene.Intersect(ray, intersection));
	EXPECT_TRUE(intersection.IsLight());
}

TEST_F(RayTraceSceneBuilderFromWorldFixture, SunLightComponentIsSkippedWithoutCrashing)
{
	auto entity = world.CreateEntity("Sun");
	entity.Add<S_SunLight>();

	RayTracing::BuildSceneFromWorld(world, scene);

	int lightCount = 0;
	scene.ForEachLight([&](const auto&) { ++lightCount; });
	EXPECT_EQ(lightCount, 0);
}

} // namespace GLEngine::Renderer
