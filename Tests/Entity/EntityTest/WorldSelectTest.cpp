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

TEST(WorldSelect, Select_NoEntities_HitsGroundPlane)
{
	C_World world;

	const Physics::Primitives::S_Ray ray{glm::vec3(0.f, 5.f, 0.f), glm::vec3(0.f, -1.f, 0.f)};
	const auto						 result = world.Select(ray);

	EXPECT_EQ(result.entityId, GUID::INVALID_GUID);
}

TEST(WorldSelect, Select_PicksNearestOfMultipleHits)
{
	C_World world;
	auto	near_								  = world.CreateEntity("Near");
	near_.Get<S_TransformComponent>().translation = glm::vec3(0.f, 0.f, 2.f);
	near_.Add<S_LocalBounds>().aabb				  = MakeAABB(glm::vec3(-0.5f), glm::vec3(0.5f));

	auto far_									 = world.CreateEntity("Far");
	far_.Get<S_TransformComponent>().translation = glm::vec3(0.f, 0.f, -2.f);
	far_.Add<S_LocalBounds>().aabb				 = MakeAABB(glm::vec3(-0.5f), glm::vec3(0.5f));
	TransformSystem::Update(world);

	const Physics::Primitives::S_Ray ray{glm::vec3(0.f, 0.f, 10.f), glm::vec3(0.f, 0.f, -1.f)};
	const auto						 result = world.Select(ray);

	EXPECT_EQ(result.entityId, near_.GetGuid());
}

} // namespace GLEngine::Entity
