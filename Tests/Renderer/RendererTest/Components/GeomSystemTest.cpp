#include <RendererTestStdafx.h>

#include <Renderer/Components/Mesh/GeomComponents.h>
#include <Renderer/Systems/GeomSystem.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/World.h>

#include "FakeRendererResources.h"

namespace GLEngine::Renderer {

namespace {

// RegisterDestroyHook points a process-global at this scope's fakeRM; without unregistering,
// that pointer would dangle once fakeRM goes out of scope at the end of the test.
class C_DestroyHookGuard {
public:
	C_DestroyHookGuard(Entity::C_World& world, ResourceManager& resourceManager)
		: m_World(world)
	{
		GeomSystem::RegisterDestroyHook(world, resourceManager);
	}
	~C_DestroyHookGuard() { GeomSystem::UnregisterDestroyHook(m_World); }

private:
	Entity::C_World& m_World;
};

} // namespace

TEST(GeomSystem, Update_BuildsGpuBuffersExactlyOnceAndAssignsADefaultMaterial)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Geom");
	entity.Add<S_GeomComponent>();

	C_FakeResourceManager fakeRM;
	C_FakeRenderer		  fakeRenderer(fakeRM);
	C_DestroyHookGuard	  destroyHookGuard(world, fakeRM);

	GeomSystem::Update(world, fakeRenderer);

	ASSERT_TRUE(entity.Has<S_GeomGPU>());
	EXPECT_NE(entity.Get<S_GeomComponent>().material, nullptr);
	EXPECT_EQ(fakeRM.createBufferCalls, 5); // positions/normals/texCoords/tangent/bitangent
	EXPECT_EQ(fakeRM.createPipelineCalls, 1);

	const int createBufferCallsAfterFirstUpdate = fakeRM.createBufferCalls;
	GeomSystem::Update(world, fakeRenderer);

	EXPECT_EQ(fakeRM.createBufferCalls, createBufferCallsAfterFirstUpdate); // already has S_GeomGPU: no rebuild
}

TEST(GeomSystem, Update_AttachesLocalBoundsMatchingThePlaneGeometry)
{
	// the old C_GeomComponent::GetAABB returned bounds built from the plane mesh; the new
	// S_GeomComponent/S_GeomGPU have no bounds field of their own, but this is not a gap - bounds
	// go through the same S_LocalBounds component every other system (StaticMesh, Lights) uses
	// for culling/picking, not a type-specific field.
	Entity::C_World world;
	auto			entity = world.CreateEntity("Geom");
	entity.Add<S_GeomComponent>();

	C_FakeResourceManager fakeRM;
	C_FakeRenderer		  fakeRenderer(fakeRM);
	C_DestroyHookGuard	  destroyHookGuard(world, fakeRM);

	GeomSystem::Update(world, fakeRenderer);

	ASSERT_TRUE(entity.Has<Entity::S_LocalBounds>());
	// MeshData::C_Geometry::CreatePlane() always builds its bbox from the hardcoded [-1,0,-1]..
	// [1,0,1] quad, regardless of the size/subdivisions parameters GeomSystem doesn't even pass.
	const auto& aabb = entity.Get<Entity::S_LocalBounds>().aabb;
	EXPECT_EQ(aabb.m_Min, glm::vec3(-1.f, 0.f, -1.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(1.f, 0.f, 1.f));
}

TEST(GeomSystem, DestroyingTheGpuComponent_FreesAllBuffersAndThePipeline)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Geom");
	entity.Add<S_GeomComponent>();

	C_FakeResourceManager fakeRM;
	C_FakeRenderer		  fakeRenderer(fakeRM);
	C_DestroyHookGuard	  destroyHookGuard(world, fakeRM);
	GeomSystem::Update(world, fakeRenderer);

	ASSERT_EQ(fakeRM.createBufferCalls, 5);
	ASSERT_EQ(fakeRM.destroyBufferCalls, 0);

	entity.Remove<S_GeomGPU>();

	EXPECT_EQ(fakeRM.destroyBufferCalls, 5);
	EXPECT_EQ(fakeRM.destroyPipelineCalls, 1);
}

TEST(GeomComponents, Geometry_IsRegisteredInComponentRegistry)
{
	EnsureGeomComponentsRegistered();

	EXPECT_NE(Entity::C_ComponentRegistry::Instance().FindByName("Geometry"), nullptr);
}

} // namespace GLEngine::Renderer
