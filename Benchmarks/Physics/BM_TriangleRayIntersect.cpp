#include <BenchmarksStdafx.h>

#include <Physics/GeometryUtils/TriangleIntersect.h>
#include <Physics/Primitives/Ray.h>

#include <glm/glm.hpp>

#include <array>

using namespace GLEngine::Physics;
using namespace GLEngine::Physics::Primitives;

namespace {
// Unit triangle in XY plane
const std::array<glm::vec3, 3> g_Triangle = {
	glm::vec3{0.f, 0.f, 0.f},
	glm::vec3{1.f, 0.f, 0.f},
	glm::vec3{0.f, 1.f, 0.f},
};

// Ray along -Z hitting the triangle centre
S_Ray g_HitRay{{0.2f, 0.2f, 5.f}, {0.f, 0.f, -1.f}};

// Ray parallel to the triangle plane (will never intersect)
S_Ray g_ParallelRay{{0.f, 0.f, 1.f}, {1.f, 0.f, 0.f}};

// Ray that misses (u/v out of range)
S_Ray g_MissRay{{5.f, 5.f, 5.f}, {0.f, 0.f, -1.f}};

S_SSERay g_SSEHitRay{g_HitRay};
S_SSERay g_SSEParallelRay{g_ParallelRay};
S_SSERay g_SSEMissRay{g_MissRay};
} // namespace

// ---------------------------------------------------------------------------
// S_Ray
// ---------------------------------------------------------------------------

static void BM_TriangleRayIntersect_Hit(benchmark::State& state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(TriangleRayIntersect(g_Triangle, g_HitRay));
}
BENCHMARK(BM_TriangleRayIntersect_Hit);

static void BM_TriangleRayIntersect_Parallel(benchmark::State& state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(TriangleRayIntersect(g_Triangle, g_ParallelRay));
}
BENCHMARK(BM_TriangleRayIntersect_Parallel);

static void BM_TriangleRayIntersect_Miss(benchmark::State& state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(TriangleRayIntersect(g_Triangle, g_MissRay));
}
BENCHMARK(BM_TriangleRayIntersect_Miss);

// ---------------------------------------------------------------------------
// S_SSERay
// ---------------------------------------------------------------------------

static void BM_TriangleRayIntersect_SSE_Hit(benchmark::State& state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(TriangleRayIntersect(g_Triangle, g_SSEHitRay));
}
BENCHMARK(BM_TriangleRayIntersect_SSE_Hit);

static void BM_TriangleRayIntersect_SSE_Parallel(benchmark::State& state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(TriangleRayIntersect(g_Triangle, g_SSEParallelRay));
}
BENCHMARK(BM_TriangleRayIntersect_SSE_Parallel);

static void BM_TriangleRayIntersect_SSE_Miss(benchmark::State& state)
{
	for (auto _ : state)
		benchmark::DoNotOptimize(TriangleRayIntersect(g_Triangle, g_SSEMissRay));
}
BENCHMARK(BM_TriangleRayIntersect_SSE_Miss);
