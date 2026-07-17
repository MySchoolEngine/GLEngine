# Benchmarks

Microbenchmarks using [Google Benchmark](https://github.com/google/benchmark).

Benchmarks are **not built by default**. Pass `--benchmarks` to premake to include them:

```
premake5 --benchmarks vs2022
```

This can be combined with other options:

```
premake5 --benchmarks --glfwapi=vulkan vs2022
```

## Running

Build and run the `Benchmarks` project in **Release** configuration for meaningful numbers (optimizer must be on):

```
msbuild Engine.sln /p:Configuration=Release /p:Platform=x64 /t:Benchmarks
bin\Release-windows-x86_64\Benchmarks\Benchmarks.exe
```

Output example:

```
--------------------------------------------------------------------
Benchmark                          Time             CPU   Iterations
--------------------------------------------------------------------
BM_AABB_Intersects_Hit           x.xx ns         x.xx ns    ...
BM_AABB_Intersects_Miss          x.xx ns         x.xx ns    ...
BM_AABB_IntersectImpl_Hit        x.xx ns         x.xx ns    ...
BM_AABB_IntersectImpl_Miss       x.xx ns         x.xx ns    ...
BM_BVH_Build/100                 x.xx us         x.xx us    ...
BM_BVH_Build/1000                x.xx us         x.xx us    ...
BM_BVH_Build/10000               x.xx ms         x.xx ms    ...
BM_BVH_Intersect_Hit/100         x.xx ns         x.xx ns    ...
BM_BVH_Intersect_Miss/100        x.xx ns         x.xx ns    ...
...
```

## Coverage

| File | What is benchmarked |
|------|---------------------|
| `Physics/BM_AABB.cpp` | `S_AABB::Intersects` (slab method) and `S_AABB::IntersectImpl` (Graphics Gems) — hit and miss rays |
| `Renderer/BM_BVH.cpp` | `BVH::Build` and `BVH::Intersect` (hit and miss) — parameterized at 100 / 1 000 / 10 000 triangles |

## Adding new benchmarks

1. Create a `.cpp` file anywhere under `Benchmarks/` — it is globbed automatically.
2. Include `<benchmark/benchmark.h>` and write benchmark functions following the [Google Benchmark guide](https://github.com/google/benchmark#usage).
3. Regenerate and rebuild.
