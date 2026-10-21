#include <BenchmarksStdafx.h>

#include <Renderer/Components/Lights/LightComponents.h>

#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/Serialization/WorldSerializer.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

#include <Core/Resources/LoadingQuery.h>
#include <Core/Resources/ResourceManager.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <pugixml.hpp>

#include <benchmark/benchmark.h>

namespace {

// Both benchmarks build each entity's matrix via the same translate*quat_cast*scale
// composition (TransformSystem::ComputeLocal's formula), not just a plain translate - otherwise
// "New" pays for quat-to-matrix conversion on every dirty entity while "Old" only ever stores a
// precomputed matrix, and the comparison mostly measures that arithmetic rather than the actual
// difference in storage/iteration. Rotation also varies per entity (not identity for all i) so a
// compiler can't hoist the quat/scale composition out of the loop as a loop invariant.
glm::quat RotationForIndex(int i)
{
	return glm::angleAxis(glm::radians(static_cast<float>(i % 360)), glm::vec3(0.f, 1.f, 0.f));
}

// ---------------------------------------------------------------------------
// New: C_World / EnTT
// ---------------------------------------------------------------------------

static void BM_WorldIterate_New_CreateAndPropagate(benchmark::State& state)
{
	const auto count = static_cast<int>(state.range(0));
	for (auto _ : state)
	{
		state.PauseTiming();
		GLEngine::Entity::C_World world;
		state.ResumeTiming();

		for (int i = 0; i < count; ++i)
		{
			auto  entity		  = world.CreateEntity("Entity");
			auto& transform		  = entity.Get<GLEngine::Entity::S_TransformComponent>();
			transform.translation = glm::vec3(static_cast<float>(i), 0.f, 0.f);
			transform.rotation	  = RotationForIndex(i);
			entity.Add<GLEngine::Entity::S_LocalBounds>();
		}
		GLEngine::Entity::TransformSystem::Update(world);

		float sum = 0.f;
		for (auto [e, transform, bounds] : world.Registry().view<GLEngine::Entity::S_WorldTransformComponent, GLEngine::Entity::S_LocalBounds>().each())
		{
			sum += transform.world[3][0];
		}
		benchmark::DoNotOptimize(sum);
	}
}
BENCHMARK(BM_WorldIterate_New_CreateAndPropagate)->Arg(1'000)->Arg(10'000);

// The old C_EntityManager/C_BasicEntity comparison benchmark (Phase 1 Task 9's baseline) was
// removed in Phase 5 along with the old ECS it measured; its recorded numbers live in that task's
// writeup, not here.

// ---------------------------------------------------------------------------
// New: real, fully-migrated level (Phase 5 Task 5's second data point)
// ---------------------------------------------------------------------------

// The plan named data/Levels/lightsTest.xml for this, but despite its name it has zero
// PointLight entities (it's a single Sponza static mesh with 393 inline materials, the known
// unconverted-C_Material deserialization issue) - using it would benchmark an empty light view.
// DeadTwistedHollow.xml has the most point lights (4) of any converted level and no inline
// materials, so it's used instead.
static void BM_WorldIterate_New_RealLevel_LightsAndMeshes(benchmark::State& state)
{
	using namespace GLEngine;

	pugi::xml_document doc;
	const auto		   result = doc.load_file("data/Levels/DeadTwistedHollow.xml");
	if (!result)
	{
		state.SkipWithError("Could not load data/Levels/DeadTwistedHollow.xml - run from the repo root.");
		return;
	}

	Entity::C_World			  world;
	Core::LoadingQuery		  query;
	Entity::C_WorldSerializer serializer;
	serializer.Load(doc, world, Core::C_ResourceManager::Instance(), query, /*loadHandlesInstantly=*/true);

	for (auto _ : state)
	{
		Entity::TransformSystem::Update(world);

		float sum = 0.f;
		for (auto [e, light, transform] : world.Registry().view<Renderer::S_PointLight, Entity::S_WorldTransformComponent>().each())
		{
			sum += transform.world[3][0] + light.color.x * light.intensity;
		}
		benchmark::DoNotOptimize(sum);
	}
}
BENCHMARK(BM_WorldIterate_New_RealLevel_LightsAndMeshes);

// ---------------------------------------------------------------------------
// New: synthetic, many-light steady state (addendum to Task 5 - every real
// converted level tops out at 4 point lights, too small to say anything about
// cache locality; this isolates pure light-view iteration cost at the same
// 1k/10k scale as the bare-registry benchmark above, so the two are
// comparable).
// ---------------------------------------------------------------------------

static void BM_WorldIterate_New_SyntheticManyLights(benchmark::State& state)
{
	using namespace GLEngine;
	const auto count = static_cast<int>(state.range(0));

	Entity::C_World world;
	for (int i = 0; i < count; ++i)
	{
		auto  entity		  = world.CreateEntity("Light");
		auto& transform		  = entity.Get<Entity::S_TransformComponent>();
		transform.translation = glm::vec3(static_cast<float>(i), 0.f, 0.f);
		transform.rotation	  = RotationForIndex(i);
		entity.Add<Renderer::S_PointLight>();
	}
	// Flush creation-time dirty tags once, outside the timed loop - every iteration below then
	// measures the steady state (nothing moved, nothing re-dirtied), matching what a real running
	// frame actually costs once the scene has settled.
	Entity::TransformSystem::Update(world);

	for (auto _ : state)
	{
		Entity::TransformSystem::Update(world);

		float sum = 0.f;
		for (auto [e, light, transform] : world.Registry().view<Renderer::S_PointLight, Entity::S_WorldTransformComponent>().each())
		{
			sum += transform.world[3][0] + light.color.x * light.intensity;
		}
		benchmark::DoNotOptimize(sum);
	}
}
BENCHMARK(BM_WorldIterate_New_SyntheticManyLights)->Arg(1'000)->Arg(10'000);

} // namespace
