#include <BenchmarksStdafx.h>

#include <Renderer/RayCasting/Geometry/Trimesh.h>
#include <Renderer/RayCasting/RayIntersection.h>

#include <Physics/Primitives/Ray.h>
#include <Physics/Primitives/Triangle.h>

#include <glm/glm.hpp>

#include <cmath>

namespace {
// Mirrors BM_BVH.cpp's grid generator, but builds triangles straight into a
// C_Trimesh (via AddTriangle) with no BVH attached, so C_Trimesh::Intersect
// takes the bruteforce path.
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

GLEngine::Renderer::C_Trimesh GenerateGridTrimesh(int triCount, bool heightfield)
{
	using namespace GLEngine::Physics::Primitives;

	const int side = GridSide(triCount);

	const auto z = [heightfield](float x, float y) { return heightfield ? HeightAt(x, y) : 0.f; };

	GLEngine::Renderer::C_Trimesh mesh;
	int							  added = 0;
	for (int i = 0; i < side && added < triCount; ++i)
	{
		for (int j = 0; j < side && added < triCount; ++j)
		{
			const float x0 = static_cast<float>(i);
			const float y0 = static_cast<float>(j);

			const glm::vec3 p00(x0, y0, z(x0, y0));
			const glm::vec3 p10(x0 + 1.f, y0, z(x0 + 1.f, y0));
			const glm::vec3 p01(x0, y0 + 1.f, z(x0, y0 + 1.f));
			const glm::vec3 p11(x0 + 1.f, y0 + 1.f, z(x0 + 1.f, y0 + 1.f));

			mesh.AddTriangle(*S_Triangle::Create(p00, p10, p01));
			++added;
			if (added >= triCount)
				break;

			mesh.AddTriangle(*S_Triangle::Create(p10, p11, p01));
			++added;
		}
	}
	return mesh;
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
} // namespace

// ---------------------------------------------------------------------------
// Trimesh bruteforce Intersect — ray hits the mesh
// ---------------------------------------------------------------------------

static void BM_Trimesh_Bruteforce_Intersect_Hit(benchmark::State& state)
{
	const int	triCount = static_cast<int>(state.range(0));
	const float side	 = static_cast<float>(GridSide(triCount));

	const auto mesh = GenerateGridTrimesh(triCount, /*heightfield=*/false);

	// Shoot straight down through the centre of the grid
	const GLEngine::Physics::Primitives::S_Ray ray{glm::vec3(side * 0.5f, side * 0.5f, 5.f), glm::vec3(0.f, 0.f, -1.f)};

	for (auto _ : state)
	{
		GLEngine::Renderer::C_RayIntersection intersection;
		benchmark::DoNotOptimize(mesh.Intersect(ray, intersection, std::numeric_limits<float>::infinity()));
	}
}
BENCHMARK(BM_Trimesh_Bruteforce_Intersect_Hit)->Arg(100)->Arg(1'000)->Arg(10'000);

// ---------------------------------------------------------------------------
// Trimesh bruteforce Intersect — ray misses the mesh entirely
// ---------------------------------------------------------------------------

static void BM_Trimesh_Bruteforce_Intersect_Miss(benchmark::State& state)
{
	const int	triCount = static_cast<int>(state.range(0));
	const float side	 = static_cast<float>(GridSide(triCount));

	const auto mesh = GenerateGridTrimesh(triCount, /*heightfield=*/false);

	// Shoot from far outside the grid
	const GLEngine::Physics::Primitives::S_Ray ray{glm::vec3(side * 10.f, side * 10.f, 5.f), glm::vec3(0.f, 0.f, -1.f)};

	for (auto _ : state)
	{
		GLEngine::Renderer::C_RayIntersection intersection;
		benchmark::DoNotOptimize(mesh.Intersect(ray, intersection, std::numeric_limits<float>::infinity()));
	}
}
BENCHMARK(BM_Trimesh_Bruteforce_Intersect_Miss)->Arg(100)->Arg(1'000)->Arg(10'000);

// ---------------------------------------------------------------------------
// Trimesh bruteforce Intersect — diagonal ray through a heightfield
// (non-degenerate triangles, closest-hit tracking actually matters)
// ---------------------------------------------------------------------------

static void BM_Trimesh_Bruteforce_Intersect_Heightfield(benchmark::State& state)
{
	const int	triCount = static_cast<int>(state.range(0));
	const float side	 = static_cast<float>(GridSide(triCount));

	const auto mesh = GenerateGridTrimesh(triCount, /*heightfield=*/true);

	// Diagonal ray from above one corner towards the middle of the surface
	const GLEngine::Physics::Primitives::S_Ray ray{glm::vec3(side * 0.2f, side * 0.3f, 8.f), glm::normalize(glm::vec3(0.4f, 0.3f, -1.f))};

	for (auto _ : state)
	{
		GLEngine::Renderer::C_RayIntersection intersection;
		benchmark::DoNotOptimize(mesh.Intersect(ray, intersection, std::numeric_limits<float>::infinity()));
	}
}
BENCHMARK(BM_Trimesh_Bruteforce_Intersect_Heightfield)->Arg(1'000)->Arg(10'000);

// ---------------------------------------------------------------------------
// Trimesh bruteforce Intersect — spread of 64 varied rays over the heightfield
// ---------------------------------------------------------------------------

static void BM_Trimesh_Bruteforce_Intersect_RaySpread(benchmark::State& state)
{
	const int	triCount = static_cast<int>(state.range(0));
	const float side	 = static_cast<float>(GridSide(triCount));

	const auto mesh = GenerateGridTrimesh(triCount, /*heightfield=*/true);
	const auto rays = GenerateRaySpread(side);

	for (auto _ : state)
	{
		for (const auto& ray : rays)
		{
			GLEngine::Renderer::C_RayIntersection intersection;
			benchmark::DoNotOptimize(mesh.Intersect(ray, intersection, std::numeric_limits<float>::infinity()));
		}
	}
	state.SetItemsProcessed(state.iterations() * static_cast<int64_t>(rays.size()));
}
BENCHMARK(BM_Trimesh_Bruteforce_Intersect_RaySpread)->Arg(1'000)->Arg(10'000);
