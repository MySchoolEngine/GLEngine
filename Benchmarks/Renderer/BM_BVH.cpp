#include <benchmark/benchmark.h>

#include <glm/glm.hpp>

#include <Renderer/RayCasting/Geometry/BVH.h>
#include <Renderer/RayCasting/RayIntersection.h>
#include <Physics/Primitives/Ray.h>

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
} // namespace

// ---------------------------------------------------------------------------
// BVH Build
// ---------------------------------------------------------------------------

static void BM_BVH_Build(benchmark::State& state)
{
	const int			   triCount		  = static_cast<int>(state.range(0));
	const auto			   originalStorage = GenerateGrid(triCount);

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
	const int  triCount = static_cast<int>(state.range(0));
	const float side	= static_cast<float>(GridSide(triCount));

	auto					  storage = GenerateGrid(triCount);
	GLEngine::Renderer::BVH   bvh(storage);

	// Shoot straight down through the centre of the grid
	const GLEngine::Physics::Primitives::S_Ray ray{
		glm::vec3(side * 0.5f, side * 0.5f, 5.f),
		glm::vec3(0.f, 0.f, -1.f)};

	const auto metrics = bvh.ComputeMetrics();
	state.counters["max depth"] = static_cast<double>(metrics.maxLeafDepth);
	state.counters["mean depth"] = static_cast<double>(metrics.meanLeafDepth);
	state.counters["min depth"] = static_cast<double>(metrics.minLeafDepth);
	state.counters["max tris"] = static_cast<double>(metrics.maxLeafTriangles);
	state.counters["mean tris"] = static_cast<double>(metrics.meanLeafTriangles);
	state.counters["min tris"] = static_cast<double>(metrics.minLeafTriangles);

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
	const int  triCount = static_cast<int>(state.range(0));
	const float side	= static_cast<float>(GridSide(triCount));

	auto					  storage = GenerateGrid(triCount);
	GLEngine::Renderer::BVH   bvh(storage);

	// Shoot from far outside the grid
	const GLEngine::Physics::Primitives::S_Ray ray{
		glm::vec3(side * 10.f, side * 10.f, 5.f),
		glm::vec3(0.f, 0.f, -1.f)};

	for (auto _ : state)
	{
		GLEngine::Renderer::C_RayIntersection intersection;
		benchmark::DoNotOptimize(bvh.Intersect(ray, intersection));
	}
}
BENCHMARK(BM_BVH_Intersect_Miss)->Arg(100)->Arg(1'000)->Arg(10'000);
