#include <RendererTestStdafx.h>

#include <Renderer/Components/Lights/LightComponents.h>
#include <Renderer/Systems/LightSystem.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

namespace GLEngine::Renderer {

TEST(LightSystem, Update_PointLightBoundsIsASinglePointAtWorldPosition)
{
	Entity::C_World world;
	auto			entity								   = world.CreateEntity("Point");
	entity.Get<Entity::S_TransformComponent>().translation = glm::vec3(2.f, 3.f, 4.f);
	entity.Add<S_PointLight>();
	Entity::TransformSystem::Update(world);

	LightSystem::Update(world);

	ASSERT_TRUE(entity.Has<Entity::S_LocalBounds>());
	const auto& aabb = entity.Get<Entity::S_LocalBounds>().aabb;
	EXPECT_EQ(aabb.m_Min, glm::vec3(2.f, 3.f, 4.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(2.f, 3.f, 4.f));
}

TEST(LightSystem, Update_AreaLightBoundsMatchesWidthAndHeight)
{
	Entity::C_World world;
	auto			entity	  = world.CreateEntity("Area");
	auto&			areaLight = entity.Add<S_AreaLight>();
	areaLight.width			  = 2.f;
	areaLight.height		  = 2.f;
	Entity::TransformSystem::Update(world);

	LightSystem::Update(world);

	ASSERT_TRUE(entity.Has<Entity::S_LocalBounds>());
	const auto& aabb = entity.Get<Entity::S_LocalBounds>().aabb;
	// At the identity transform, GetWorldNormal() is -Z and GetWorldUpVector() is +Y, so
	// dirX = cross(normal, up) is +X. GetAABB's half-extents are sqrt(width/2) and
	// sqrt(height/2) (see LightFunctions.cpp, not this finding's concern), so a 2x2
	// light's extents are both sqrt(1) = 1, landing the corners at (+-1, +-1, 0).
	EXPECT_NEAR(aabb.m_Min.x, -1.f, 0.0001f);
	EXPECT_NEAR(aabb.m_Min.y, -1.f, 0.0001f);
	EXPECT_NEAR(aabb.m_Min.z, 0.f, 0.0001f);
	EXPECT_NEAR(aabb.m_Max.x, 1.f, 0.0001f);
	EXPECT_NEAR(aabb.m_Max.y, 1.f, 0.0001f);
	EXPECT_NEAR(aabb.m_Max.z, 0.f, 0.0001f);
}

TEST(LightSystem, Update_SunLightBoundsStaysUninitialized)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Sun");
	entity.Add<S_SunLight>();
	Entity::TransformSystem::Update(world);

	LightSystem::Update(world);

	// GetAABB(S_SunLight, ...) always returns a default-constructed (uninitialized) S_AABB - a
	// sun light has no finite position-based bounds. This pins down that known behavior rather
	// than asserting a real bound, so a future change to real bounds will be a deliberate,
	// visible test update instead of a silent behavior change.
	ASSERT_TRUE(entity.Has<Entity::S_LocalBounds>());
	EXPECT_FALSE(entity.Get<Entity::S_LocalBounds>().aabb.IsInitialized());
}

TEST(LightSystem, Update_EntityWithoutWorldTransform_IsSkipped)
{
	Entity::C_World world;
	auto			entity = world.CreateEntity("Point");
	entity.Add<S_PointLight>();
	entity.Remove<Entity::S_WorldTransformComponent>();

	LightSystem::Update(world);

	EXPECT_FALSE(entity.Has<Entity::S_LocalBounds>());
}

TEST(LightSystem, Update_MovingALight_RefreshesItsBounds)
{
	Entity::C_World world;
	auto			entity								   = world.CreateEntity("Point");
	entity.Get<Entity::S_TransformComponent>().translation = glm::vec3(1.f, 0.f, 0.f);
	entity.Add<S_PointLight>();
	Entity::TransformSystem::Update(world);
	LightSystem::Update(world);

	ASSERT_TRUE(entity.Has<Entity::S_LocalBounds>());
	EXPECT_EQ(entity.Get<Entity::S_LocalBounds>().aabb.m_Min, glm::vec3(1.f, 0.f, 0.f));

	// Mutating the component through Get() doesn't fire C_World's on_update<S_TransformComponent>
	// signal, so Patch() (or a dirty tag) must be used explicitly for TransformSystem::Update to
	// see it as changed (the first move above only worked because entity creation unconditionally
	// seeds one).
	entity.Get<Entity::S_TransformComponent>().translation = glm::vec3(5.f, 6.f, 7.f);
	entity.Patch<Entity::S_TransformComponent>();
	Entity::TransformSystem::Update(world);
	LightSystem::Update(world);

	const auto& aabb = entity.Get<Entity::S_LocalBounds>().aabb;
	EXPECT_EQ(aabb.m_Min, glm::vec3(5.f, 6.f, 7.f));
	EXPECT_EQ(aabb.m_Max, glm::vec3(5.f, 6.f, 7.f));
}

} // namespace GLEngine::Renderer
