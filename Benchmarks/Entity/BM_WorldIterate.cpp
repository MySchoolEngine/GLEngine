#include <BenchmarksStdafx.h>

#include <Entity/BasicEntity.h>
#include <Entity/Components/CoreComponents.h>
#include <Entity/Entity.h>
#include <Entity/EntityManager.h>
#include <Entity/Systems/TransformSystem.h>
#include <Entity/World.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

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

// ---------------------------------------------------------------------------
// Old: C_EntityManager / shared_ptr<C_BasicEntity>
// ---------------------------------------------------------------------------

static void BM_WorldIterate_Old_CreateAndIterate(benchmark::State& state)
{
	const auto count = static_cast<int>(state.range(0));
	for (auto _ : state)
	{
		state.PauseTiming();
		GLEngine::Entity::C_EntityManager manager;
		state.ResumeTiming();

		for (int i = 0; i < count; ++i)
		{
			auto	   entity	= std::make_shared<GLEngine::Entity::C_BasicEntity>("Entity");
			const auto rotation = RotationForIndex(i);
			const auto matrix
				= glm::translate(glm::mat4(1.f), glm::vec3(static_cast<float>(i), 0.f, 0.f)) * glm::mat4_cast(rotation) * glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));
			entity->SetModelMatrix(matrix);
			manager.AddEntity(entity);
		}

		float sum = 0.f;
		for (const auto& entity : manager.GetEntities())
		{
			sum += entity->GetModelMatrix()[3][0];
		}
		benchmark::DoNotOptimize(sum);
	}
}
BENCHMARK(BM_WorldIterate_Old_CreateAndIterate)->Arg(1'000)->Arg(10'000);

} // namespace
