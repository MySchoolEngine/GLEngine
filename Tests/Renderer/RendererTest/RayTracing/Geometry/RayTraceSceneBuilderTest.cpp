#include <RendererTestStdafx.h>

#include <Renderer/Colours.h>
#include <Renderer/Components/StaticMeshHandles.h>
#include <Renderer/Lights/AreaLight.h>
#include <Renderer/Lights/PointLight.h>
#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RayCasting/Geometry/RayTraceScene.h>
#include <Renderer/RayCasting/Geometry/RayTraceSceneBuilder.h>
#include <Renderer/RayCasting/Light/ILight.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Renderer/RayCasting/VisibilityTester.h>

#include <Physics/Primitives/Frustum.h>
#include <Physics/Primitives/Ray.h>

#include <Entity/BasicEntity.h>
#include <Entity/EntityManager.h>

#include <glm/gtx/transform.hpp>

namespace GLEngine::Renderer {

namespace {

// Minimal light component with no ray-trace equivalent, used to verify the builder skips
// unsupported components instead of asserting/throwing.
class C_DummyUnsupportedLight : public I_Light {
public:
	explicit C_DummyUnsupportedLight(const std::shared_ptr<Entity::I_Entity>& owner)
		: I_Light(owner)
	{
	}
	[[nodiscard]] Physics::Primitives::C_Frustum GetShadingFrustum() const override
	{
		return Physics::Primitives::C_Frustum(glm::vec3(0.f), glm::vec3(0.f), glm::vec3(0.f), 1.f, 1.f, 1.f, 1.f);
	}
	[[nodiscard]] bool						  HasDebugDrawGUI() const override { return false; }
	[[nodiscard]] std::string_view			  GetDebugComponentName() const override { return "DummyUnsupportedLight"; }
	[[nodiscard]] Physics::Primitives::S_AABB GetAABB() const override { return {}; }
};

} // namespace

// ============================================================================
// RayTraceSceneBuilder::BuildSceneFromEntityManager
// ============================================================================

class RayTraceSceneBuilderFixture : public ::testing::Test {
protected:
	Entity::C_EntityManager manager;
	C_RayTraceScene			scene;
};

TEST_F(RayTraceSceneBuilderFixture, MeshComponentWithoutReadyResourceIsSkippedWithoutCrashing)
{
	auto entity = std::make_shared<Entity::C_BasicEntity>("Mesh");
	auto mesh	= std::make_shared<C_StaticMeshHandles>(); // no SetMeshFile() called, so the underlying MeshResource is never ready
	mesh->SetParent(entity);
	entity->AddComponent(mesh);
	manager.AddEntity(entity);

	RayTracing::BuildSceneFromEntityManager(manager, scene);

	int lightCount = 0;
	scene.ForEachLight([&](const auto&) { ++lightCount; });
	EXPECT_EQ(lightCount, 0);
}

TEST_F(RayTraceSceneBuilderFixture, PointLightIsConvertedWithWorldPositionAndIntensity)
{
	auto entity = std::make_shared<Entity::C_BasicEntity>("PointLight");
	entity->SetModelMatrix(glm::translate(glm::mat4(1.f), glm::vec3(1, 2, 3)));
	auto light = std::make_shared<C_PointLight>(entity); // ctor already sets the owner, no separate SetParent
	entity->AddComponent(light);
	manager.AddEntity(entity);

	RayTracing::BuildSceneFromEntityManager(manager, scene);

	int deltaLightCount = 0;
	scene.ForEachLight([&](const std::reference_wrapper<const RayTracing::I_RayLight>& l) {
		if (l.get().IsDeltaLight())
			++deltaLightCount;
	});
	EXPECT_EQ(deltaLightCount, 1);

	// SampleLi from a point right above the light's position should report a positive radiance,
	// confirming the light was placed at the entity's world position (1,2,3).
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

TEST_F(RayTraceSceneBuilderFixture, AreaLightGeometryIsIntersectableAtItsWorldTransform)
{
	auto entity = std::make_shared<Entity::C_BasicEntity>("AreaLight");
	entity->SetModelMatrix(glm::translate(glm::mat4(1.f), glm::vec3(0, 5, 0)));
	auto light = std::make_shared<C_AreaLight>(entity); // ctor already sets the owner, no separate SetParent
	entity->AddComponent(light);
	manager.AddEntity(entity);

	RayTracing::BuildSceneFromEntityManager(manager, scene);

	int areaLightCount = 0;
	scene.ForEachLight([&](const std::reference_wrapper<const RayTracing::I_RayLight>& l) {
		if (!l.get().IsDeltaLight())
			++areaLightCount;
	});
	ASSERT_EQ(areaLightCount, 1);

	// The area light's default normal/up (before any rotation) face -Z and +Y respectively (see
	// C_AreaLight::GetNormal/GetUpVector), so the rectangle plane sits at the entity's translation
	// with normal (0,0,-1). One-sided emitters only register hits from the side the normal points
	// toward, so the ray must approach from -Z heading in +Z.
	constexpr Physics::Primitives::S_Ray ray(glm::vec3(0, 5, -5), glm::vec3(0, 0, 1));
	C_RayIntersection		   intersection;
	ASSERT_TRUE(scene.Intersect(ray, intersection));
	EXPECT_TRUE(intersection.IsLight());
}

TEST_F(RayTraceSceneBuilderFixture, UnsupportedLightComponentIsSkippedWithoutCrashing)
{
	auto entity = std::make_shared<Entity::C_BasicEntity>("Unsupported");
	auto light	= std::make_shared<C_DummyUnsupportedLight>(entity); // ctor already sets the owner, no separate SetParent
	entity->AddComponent(light);
	manager.AddEntity(entity);

	RayTracing::BuildSceneFromEntityManager(manager, scene);

	int lightCount = 0;
	scene.ForEachLight([&](const auto&) { ++lightCount; });
	EXPECT_EQ(lightCount, 0);
}

} // namespace GLEngine::Renderer
