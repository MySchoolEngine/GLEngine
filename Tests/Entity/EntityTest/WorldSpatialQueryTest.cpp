#include <EntityTestStdafx.h>

#include <Physics/Primitives/Sphere.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

namespace GLEngine::Entity {

namespace {
Physics::Primitives::S_AABB MakeAABB(const glm::vec3& min, const glm::vec3& max)
{
	Physics::Primitives::S_AABB aabb;
	aabb.m_Min = min;
	aabb.m_Max = max;
	return aabb;
}
} // namespace

TEST(WorldSpatialQuery, ForEachInSphere_VisitsEntityWhoseBoundsOverlapTheSphere)
{
	C_World world;
	auto	entity								   = world.CreateEntity("Target");
	entity.Get<S_TransformComponent>().translation = glm::vec3(0.f, 0.f, 0.f);
	entity.Add<S_LocalBounds>().aabb			   = MakeAABB(glm::vec3(-1.f), glm::vec3(1.f));
	TransformSystem::Update(world);

	const Physics::Primitives::S_Sphere querySphere(glm::vec3(0.f), 5.f);

	std::vector<GUID> visited;
	world.ForEachInSphere(querySphere, [&](C_Entity e) { visited.push_back(e.GetGuid()); });

	ASSERT_EQ(visited.size(), 1u);
	EXPECT_EQ(visited[0], entity.GetGuid());
}

TEST(WorldSpatialQuery, ForEachInSphere_SkipsEntityOutsideTheSphere)
{
	C_World world;
	auto	entity								   = world.CreateEntity("Far");
	entity.Get<S_TransformComponent>().translation = glm::vec3(100.f, 0.f, 0.f);
	entity.Add<S_LocalBounds>().aabb			   = MakeAABB(glm::vec3(-1.f), glm::vec3(1.f));
	TransformSystem::Update(world);

	// Query sphere sits at the origin with a radius far short of reaching the entity at x=100.
	const Physics::Primitives::S_Sphere querySphere(glm::vec3(0.f), 5.f);

	int callCount = 0;
	world.ForEachInSphere(querySphere, [&](C_Entity) { ++callCount; });

	EXPECT_EQ(callCount, 0);
}

TEST(WorldSpatialQuery, ForEachInSphere_VisitsEveryMatchingEntityNotJustTheFirst)
{
	C_World world;

	auto a							   = world.CreateEntity("A");
	a.Get<S_TransformComponent>().translation = glm::vec3(-2.f, 0.f, 0.f);
	a.Add<S_LocalBounds>().aabb	   = MakeAABB(glm::vec3(-0.5f), glm::vec3(0.5f));

	auto b							   = world.CreateEntity("B");
	b.Get<S_TransformComponent>().translation = glm::vec3(2.f, 0.f, 0.f);
	b.Add<S_LocalBounds>().aabb	   = MakeAABB(glm::vec3(-0.5f), glm::vec3(0.5f));

	// Outside the query sphere below - must not be visited.
	auto c							   = world.CreateEntity("C");
	c.Get<S_TransformComponent>().translation = glm::vec3(100.f, 0.f, 0.f);
	c.Add<S_LocalBounds>().aabb	   = MakeAABB(glm::vec3(-0.5f), glm::vec3(0.5f));
	TransformSystem::Update(world);

	const Physics::Primitives::S_Sphere querySphere(glm::vec3(0.f), 5.f);

	std::vector<GUID> visited;
	world.ForEachInSphere(querySphere, [&](C_Entity e) { visited.push_back(e.GetGuid()); });

	ASSERT_EQ(visited.size(), 2u);
	EXPECT_NE(std::find(visited.begin(), visited.end(), a.GetGuid()), visited.end());
	EXPECT_NE(std::find(visited.begin(), visited.end(), b.GetGuid()), visited.end());
}

TEST(WorldSpatialQuery, ForEachInSphere_NoEntitiesWithBounds_NeverCallsFunc)
{
	C_World world;
	world.CreateEntity("NoBounds"); // has a transform but no S_LocalBounds

	const Physics::Primitives::S_Sphere querySphere(glm::vec3(0.f), 1000.f);

	int callCount = 0;
	world.ForEachInSphere(querySphere, [&](C_Entity) { ++callCount; });

	EXPECT_EQ(callCount, 0);
}

} // namespace GLEngine::Entity
