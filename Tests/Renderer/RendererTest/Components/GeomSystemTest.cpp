#include <RendererTestStdafx.h>

#include <Renderer/Components/Mesh/GeomComponents.h>
#include <Renderer/Systems/GeomSystem.h>

#include <Entity/ComponentRegistry.h>
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
