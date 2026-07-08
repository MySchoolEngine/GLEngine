#include <Renderer/RayCasting/Geometry/BVH.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/Primitives/Ray.h>

#include <glm/glm.hpp>

#include <benchmark/benchmark.h>
#include <cmath>
#include <vector>

namespace {
// Generate a flat grid of triangles lying in the XY plane.
// Each square cell produces 2 triangles.  The grid spans [0, side) x [0, side).
std::vector<glm::vec3> GenerateGrid(int triCount)
{
	const int cells = std::max(1, triCount / 2);
	const int side	= static_cast<int>(std::ceil(std::sqrt(static_cast<double>(cells))));

	std::vector<glm::vec3> verts;
	verts.reserve(triCount * 3);

	for (int i = 0; i < side; ++i)
	{
		for (int j = 0; j < side; ++j)
		{
			if (static_cast<int>(verts.size()) >= triCount * 3)
				break;
			const float x0 = static_cast<float>(i);
			const float y0 = static_cast<float>(j);
			// Triangle 1
			verts.emplace_back(x0, y0, 0.f);
			verts.emplace_back(x0 + 1.f, y0, 0.f);
			verts.emplace_back(x0, y0 + 1.f, 0.f);
			// Triangle 2
			verts.emplace_back(x0 + 1.f, y0, 0.f);
			verts.emplace_back(x0 + 1.f, y0 + 1.f, 0.f);
			verts.emplace_back(x0, y0 + 1.f, 0.f);
		}
	}
	return verts;
}

int GridSide(int triCount)
{
	const int cells = std::max(1, triCount / 2);
	return static_cast<int>(std::ceil(std::sqrt(static_cast<double>(cells))));
}

// Heightfield displacement: z depends only on (x, y), so duplicated vertices
// of neighbouring triangles receive identical heights and the surface stays
// watertight.
float HeightAt(float x, float y)
{
	return std::sin(x * 0.4f) * std::cos(y * 0.4f) * 3.f;
}

// Same grid as GenerateGrid but displaced in z — node AABBs overlap in depth,
// so traversal order and pruning actually matter (a flat grid is degenerate for
// both).
std::vector<glm::vec3> GenerateHeightfield(int triCount)
{
	auto verts = GenerateGrid(triCount);
	for (auto& v : verts)
		v.z = HeightAt(v.x, v.y);
	return verts;
}

// 64 deterministic rays spread over the grid footprint: varied origins above
// the mesh, tilted downward directions. Index-derived (golden-angle) — no RNG,
// stable across runs.
std::vector<GLEngine::Physics::Primitives::S_Ray> GenerateRaySpread(float side)
{
	constexpr int									  rayCount = 64;
	std::vector<GLEngine::Physics::Primitives::S_Ray> rays;
	rays.reserve(rayCount);
	for (int k = 0; k < rayCount; ++k)
	{
		const float t	  = static_cast<float>(k) / (rayCount - 1);
		const float angle = static_cast<float>(k) * 2.399963f; // golden angle
		const float fracY = static_cast<float>(std::fmod(static_cast<double>(k) * 0.618, 1.0));

		const glm::vec3 origin(side * (0.15f + 0.7f * t), side * (0.15f + 0.7f * fracY), 6.f + static_cast<float>(k % 5));
		const glm::vec3 direction = glm::normalize(glm::vec3(std::sin(angle) * 0.5f, std::cos(angle) * 0.5f, -1.f));
		rays.emplace_back(origin, direction);
	}
	return rays;
}

static void AddMetrics(benchmark::State& state, const GLEngine::Renderer::BVH& bvh)
{
	const auto metrics			 = bvh.ComputeMetrics();
	state.counters["max depth"]	 = static_cast<double>(metrics.maxLeafDepth);
	state.counters["mean depth"] = static_cast<double>(metrics.meanLeafDepth);
	state.counters["min depth"]	 = static_cast<double>(metrics.minLeafDepth);
	state.counters["max tris"]	 = static_cast<double>(metrics.maxLeafTriangles);
	state.counters["mean tris"]	 = static_cast<double>(metrics.meanLeafTriangles);
	state.counters["min tris"]	 = static_cast<double>(metrics.minLeafTriangles);
}
} // namespace

// ---------------------------------------------------------------------------
// BVH Build
// ---------------------------------------------------------------------------

static void BM_BVH_Build(benchmark::State& state)
{
	const int  triCount		   = static_cast<int>(state.range(0));
	const auto originalStorage = GenerateGrid(triCount);

	for (auto _ : state)
	{
		state.PauseTiming();
		auto storage = originalStorage; // fresh copy — Build() reorders elements
		state.ResumeTiming();

		GLEngine::Renderer::BVH bvh(storage);
		benchmark::DoNotOptimize(bvh);
	}
}
BENCHMARK(BM_BVH_Build)->Arg(100)->Arg(1'000)->Arg(10'000);

// ---------------------------------------------------------------------------
// BVH Intersect — ray hits the mesh
// ---------------------------------------------------------------------------
static void BM_BVH_Intersect_Hit(benchmark::State& state)
{
	const int	triCount = static_cast<int>(state.range(0));
	const float side	 = static_cast<float>(GridSide(triCount));

	auto					storage = GenerateGrid(triCount);
	GLEngine::Renderer::BVH bvh(storage);

	// Shoot straight down through the centre of the grid
	const GLEngine::Physics::Primitives::S_Ray ray{glm::vec3(side * 0.5f, side * 0.5f, 5.f), glm::vec3(0.f, 0.f, -1.f)};

	AddMetrics(state, bvh);

	for (auto _ : state)
	{
		GLEngine::Renderer::C_RayIntersection intersection;
		benchmark::DoNotOptimize(bvh.Intersect(ray, intersection));
	}
}
BENCHMARK(BM_BVH_Intersect_Hit)->Arg(100)->Arg(1'000)->Arg(10'000);

// ---------------------------------------------------------------------------
// BVH Intersect — ray misses the mesh entirely
// ---------------------------------------------------------------------------

static void BM_BVH_Intersect_Miss(benchmark::State& state)
{
	const int	triCount = static_cast<int>(state.range(0));
	const float side	 = static_cast<float>(GridSide(triCount));

	auto					storage = GenerateGrid(triCount);
	GLEngine::Renderer::BVH bvh(storage);

	// Shoot from far outside the grid
	const GLEngine::Physics::Primitives::S_Ray ray{glm::vec3(side * 10.f, side * 10.f, 5.f), glm::vec3(0.f, 0.f, -1.f)};

	for (auto _ : state)
	{
		GLEngine::Renderer::C_RayIntersection intersection;
		benchmark::DoNotOptimize(bvh.Intersect(ray, intersection));
	}
}
BENCHMARK(BM_BVH_Intersect_Miss)->Arg(100)->Arg(1'000)->Arg(10'000);

// ---------------------------------------------------------------------------
// BVH Intersect — diagonal ray through a heightfield (non-degenerate AABBs)
// ---------------------------------------------------------------------------

static void BM_BVH_Intersect_Heightfield(benchmark::State& state)
{
	const int	triCount = static_cast<int>(state.range(0));
	const float side	 = static_cast<float>(GridSide(triCount));

	auto					storage = GenerateHeightfield(triCount);
	GLEngine::Renderer::BVH bvh(storage);
	AddMetrics(state, bvh);

	// Diagonal ray from above one corner towards the middle of the surface
	const GLEngine::Physics::Primitives::S_Ray ray{glm::vec3(side * 0.2f, side * 0.3f, 8.f), glm::normalize(glm::vec3(0.4f, 0.3f, -1.f))};

	for (auto _ : state)
	{
		GLEngine::Renderer::C_RayIntersection intersection;
		benchmark::DoNotOptimize(bvh.Intersect(ray, intersection));
	}
}
BENCHMARK(BM_BVH_Intersect_Heightfield)->Arg(1'000)->Arg(10'000);

// ---------------------------------------------------------------------------
// BVH Intersect — spread of 64 varied rays over the heightfield
// ---------------------------------------------------------------------------

static void BM_BVH_Intersect_RaySpread(benchmark::State& state)
{
	const int	triCount = static_cast<int>(state.range(0));
	const float side	 = static_cast<float>(GridSide(triCount));

	auto					storage = GenerateHeightfield(triCount);
	GLEngine::Renderer::BVH bvh(storage);
	AddMetrics(state, bvh);

	const auto rays = GenerateRaySpread(side);

	for (auto _ : state)
	{
		for (const auto& ray : rays)
		{
			GLEngine::Renderer::C_RayIntersection intersection;
			benchmark::DoNotOptimize(bvh.Intersect(ray, intersection));
		}
	}
	state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(rays.size()));
}
BENCHMARK(BM_BVH_Intersect_RaySpread)->Arg(1'000)->Arg(10'000);
