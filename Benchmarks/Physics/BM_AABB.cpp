#include <benchmark/benchmark.h>

#include <glm/glm.hpp>

#include <Physics/Primitives/AABB.h>
#include <Physics/Primitives/Ray.h>

using namespace GLEngine::Physics::Primitives;

namespace {
S_AABB MakeUnitBox()
{
	S_AABB aabb;
	aabb.Add(glm::vec3(-1.f, -1.f, -1.f));
	aabb.Add(glm::vec3(1.f, 1.f, 1.f));
	return aabb;
}

// Ray along +Z that passes through the centre of the unit box
S_Ray g_HitRay{{0.f, 0.f, -5.f}, {0.f, 0.f, 1.f}};

// Ray along +Z that completely misses the unit box
S_Ray g_MissRay{{10.f, 0.f, -5.f}, {0.f, 0.f, 1.f}};

S_SSERay g_SSEHitRay{ g_HitRay };
S_SSERay g_SSEMissRay{ g_MissRay };
} // namespace

// ---------------------------------------------------------------------------
// Intersects (slab method)
// ---------------------------------------------------------------------------

static void BM_AABB_Intersects_Hit(benchmark::State& state)
{
	const S_AABB aabb = MakeUnitBox();
	for (auto _ : state)
		benchmark::DoNotOptimize(aabb.Intersects(g_HitRay));
}
BENCHMARK(BM_AABB_Intersects_Hit);

static void BM_AABB_Intersects_Miss(benchmark::State& state)
{
	const S_AABB aabb = MakeUnitBox();
	for (auto _ : state)
		benchmark::DoNotOptimize(aabb.Intersects(g_MissRay));
}
BENCHMARK(BM_AABB_Intersects_Miss);

static void BM_AABB_SSE_Intersects_Hit(benchmark::State& state)
{
	const S_AABB aabb = MakeUnitBox();
	for (auto _ : state)
		benchmark::DoNotOptimize(aabb.Intersects(g_SSEHitRay));
}
BENCHMARK(BM_AABB_SSE_Intersects_Hit);

static void BM_AABB_SSE_Intersects_Miss(benchmark::State& state)
{
	const S_AABB aabb = MakeUnitBox();
	for (auto _ : state)
		benchmark::DoNotOptimize(aabb.Intersects(g_SSEMissRay));
}
BENCHMARK(BM_AABB_SSE_Intersects_Miss);

// ---------------------------------------------------------------------------
// IntersectImpl (Graphics Gems method)
// ---------------------------------------------------------------------------

static void BM_AABB_IntersectImpl_Hit(benchmark::State& state)
{
	const S_AABB aabb = MakeUnitBox();
	for (auto _ : state)
		benchmark::DoNotOptimize(aabb.IntersectImpl(g_HitRay));
}
BENCHMARK(BM_AABB_IntersectImpl_Hit);

static void BM_AABB_IntersectImpl_Miss(benchmark::State& state)
{
	const S_AABB aabb = MakeUnitBox();
	for (auto _ : state)
		benchmark::DoNotOptimize(aabb.IntersectImpl(g_MissRay));
}
BENCHMARK(BM_AABB_IntersectImpl_Miss);
