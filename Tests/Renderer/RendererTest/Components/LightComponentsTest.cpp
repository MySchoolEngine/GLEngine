#include <RendererTestStdafx.h>

#include <Renderer/Components/Lights/LightComponents.h>
#include <Renderer/Components/Lights/LightFunctions.h>
#include <Renderer/Systems/LightSystem.h>

#include <Entity/ComponentRegistry.h>
#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Serialization/WorldSerializer.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

#include <pugixml.hpp>

namespace GLEngine::Renderer {

TEST(LightComponents, PointLight_GetAABB_ContainsWorldPosition)
{
	Entity::C_World world;
	auto			entity								   = world.CreateEntity("Light");
	entity.Get<Entity::S_TransformComponent>().translation = glm::vec3(2.f, 3.f, 4.f);
	entity.Add<S_PointLight>();
	Entity::TransformSystem::Update(world);

	const auto aabb = GetAABB(entity.Get<S_PointLight>(), entity.Get<Entity::S_WorldTransformComponent>());

	EXPECT_TRUE(aabb.Contains(glm::vec3(2.f, 3.f, 4.f)));
}

TEST(LightComponents, PointLight_GetShadingFrustum_AtWorldPosition)
{
	Entity::C_World world;
	auto			entity								   = world.CreateEntity("Light");
	entity.Get<Entity::S_TransformComponent>().translation = glm::vec3(1.f, 2.f, 3.f);
	entity.Add<S_PointLight>();
	Entity::TransformSystem::Update(world);

	const auto frustum = GetShadingFrustum(entity.Get<S_PointLight>(), entity.Get<Entity::S_WorldTransformComponent>());

	EXPECT_EQ(frustum.GetPosition(), glm::vec3(1.f, 2.f, 3.f));
}

TEST(LightComponents, AreaLight_GetAABB_IsSymmetricAroundOrigin)
{
	Entity::C_World world;
	auto			entity	  = world.CreateEntity("Light");
	auto&			areaLight = entity.Add<S_AreaLight>();
	areaLight.width			  = 2.f;
	areaLight.height		  = 2.f;
	Entity::TransformSystem::Update(world);

	const auto aabb = GetAABB(entity.Get<S_AreaLight>(), entity.Get<Entity::S_WorldTransformComponent>());

	EXPECT_NEAR(aabb.m_Min.x, -aabb.m_Max.x, 0.0001f);
	EXPECT_NEAR(aabb.m_Min.y, -aabb.m_Max.y, 0.0001f);
}

TEST(LightComponents, SunLight_GetSunDirection_MatchesStoredFields)
{
	Entity::C_World world;
	auto			entity	 = world.CreateEntity("Sun");
	auto&			sunLight = entity.Add<S_SunLight>();
	sunLight.sunX			 = 0.1f;
	sunLight.sunY			 = 0.2f;
	sunLight.sunZ			 = -0.3f;

	EXPECT_EQ(sunLight.sunX, 0.1f);
	EXPECT_EQ(sunLight.sunY, 0.2f);
	EXPECT_EQ(sunLight.sunZ, -0.3f);
}

TEST(LightSystem, Update_PopulatesLocalBoundsForAllThreeLightTypes)
{
	Entity::C_World world;
	auto			point = world.CreateEntity("Point");
	point.Add<S_PointLight>();
	auto area = world.CreateEntity("Area");
	area.Add<S_AreaLight>();
	auto sun = world.CreateEntity("Sun");
	sun.Add<S_SunLight>();
	Entity::TransformSystem::Update(world);

	LightSystem::Update(world);

	EXPECT_TRUE(point.Has<Entity::S_LocalBounds>());
	EXPECT_TRUE(area.Has<Entity::S_LocalBounds>());
	EXPECT_TRUE(sun.Has<Entity::S_LocalBounds>());
}

TEST(LightComponents, AllThreeLightTypes_AreRegisteredInComponentRegistry)
{
	EnsureLightComponentsRegistered();

	EXPECT_NE(Entity::C_ComponentRegistry::Instance().FindByName("PointLight"), nullptr);
	EXPECT_NE(Entity::C_ComponentRegistry::Instance().FindByName("AreaLight"), nullptr);
	EXPECT_NE(Entity::C_ComponentRegistry::Instance().FindByName("SunLight"), nullptr);
}

TEST(LightComponents, SunLight_SaveThenLoad_PreservesAsymmetricFactorAndDiscMultiplierIndependently)
{
	EnsureLightComponentsRegistered();

	Entity::C_World world;
	auto			entity	   = world.CreateEntity("Sun");
	auto&			sunLight   = entity.Add<S_SunLight>();
	sunLight.asymmetricFactor  = 0.75f;
	sunLight.sunDiscMultiplier = 2.5f;
	const GUID guid			   = entity.GetGuid();

	Entity::C_WorldSerializer serializer;
	pugi::xml_document		  doc = serializer.Save(world);

	Entity::C_World	   loaded;
	Core::LoadingQuery query;
	serializer.Load(doc, loaded, Core::C_ResourceManager::Instance(), query, true);

	auto loadedEntity = loaded.FindByGuid(guid);
	ASSERT_TRUE(loadedEntity.IsValid());
	const auto& loadedSun = loadedEntity.Get<S_SunLight>();
	EXPECT_FLOAT_EQ(loadedSun.asymmetricFactor, 0.75f);
	EXPECT_FLOAT_EQ(loadedSun.sunDiscMultiplier, 2.5f);
}

} // namespace GLEngine::Renderer
