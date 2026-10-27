#include <EntityTestStdafx.h>

#include <Physics/Primitives/Ray.h>

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

TEST(WorldSelect, Select_HitsEntityWithBounds)
{
	C_World world;
	auto	entity								   = world.CreateEntity("Target");
	entity.Get<S_TransformComponent>().translation = glm::vec3(0.f, 0.f, 0.f);
	entity.Add<S_LocalBounds>().aabb			   = MakeAABB(glm::vec3(-1.f), glm::vec3(1.f));
	TransformSystem::Update(world);

	const Physics::Primitives::S_Ray ray{glm::vec3(0.f, 0.f, 5.f), glm::vec3(0.f, 0.f, -1.f)};
	const auto						 result = world.Select(ray);

	EXPECT_EQ(result.entityId, entity.GetGuid());
}

TEST(WorldSelect, Select_RayMissesBoundedEntity_ReturnsInvalid)
{
	C_World world;
	auto	entity								   = world.CreateEntity("Target");
	entity.Get<S_TransformComponent>().translation = glm::vec3(0.f, 0.f, 0.f);
	entity.Add<S_LocalBounds>().aabb			   = MakeAABB(glm::vec3(-1.f), glm::vec3(1.f));
	TransformSystem::Update(world);

	// Starts well outside the entity's [-1,1]^3 bounds and travels further away on every axis -
	// an "always hit" AABB intersection would still wrongly return this entity.
	const Physics::Primitives::S_Ray ray{glm::vec3(10.f, 10.f, 10.f), glm::vec3(1.f, 0.f, 0.f)};
	const auto						 result = world.Select(ray);

	EXPECT_EQ(result.entityId, GUID::INVALID_GUID);
}

TEST(WorldSelect, Select_NoEntities_HitsGroundPlane)
{
	C_World world;

	const Physics::Primitives::S_Ray ray{glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, -1.f, 0.f)};
	const auto						 result = world.Select(ray);

	EXPECT_EQ(result.entityId, GUID::INVALID_GUID);
	// The fallback ground plane sits at y=-1 (constructed as normal (0,1,0), offset -1, so
	// origin = normal*offset = (0,-1,0)); a ray starting at y=5 travelling straight down covers
	// a distance of 6 to reach it.
	EXPECT_NEAR(result.distance, 6.f, 0.0001f);
	EXPECT_NEAR(result.intersectionPoint.y, -1.f, 0.0001f);
}

TEST(WorldSelect, Select_PicksNearestOfMultipleHits)
{
	C_World world;

	// create far first if the result depended on iteration order
	auto far_									  = world.CreateEntity("Far");
	far_.Get<S_TransformComponent>().translation  = glm::vec3(0.f, 0.f, -2.f);
	far_.Add<S_LocalBounds>().aabb				  = MakeAABB(glm::vec3(-0.5f), glm::vec3(0.5f));

	auto	near_								  = world.CreateEntity("Near");
	near_.Get<S_TransformComponent>().translation = glm::vec3(0.f, 0.f, 2.f);
	near_.Add<S_LocalBounds>().aabb				  = MakeAABB(glm::vec3(-0.5f), glm::vec3(0.5f));
	TransformSystem::Update(world);

	const Physics::Primitives::S_Ray ray{glm::vec3(0.f, 0.f, 10.f), glm::vec3(0.f, 0.f, -1.f)};
	const auto						 result = world.Select(ray);

	EXPECT_EQ(result.entityId, near_.GetGuid());
	// Near's bounds are centered at z=2 with a half-extent of 0.5, so its closest face to the ray
	// origin (z=10) is at z=2.5 - a distance of 7.5. A broken min_element that returned Far's hit
	// (centered at z=-2, distance 12.5) would fail this even if entityId happened to match.
	EXPECT_NEAR(result.distance, 7.5f, 0.0001f);
}

} // namespace GLEngine::Entity
