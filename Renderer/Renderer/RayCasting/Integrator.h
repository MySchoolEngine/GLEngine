#pragma once

#include <Renderer/Colours.h>

#include <Physics/Primitives/Ray.h>

#include <Utils/Allocation/StdStackAllocator.h>

namespace GLEngine::Renderer {
class I_Sampler;
class C_RayTraceScene;
class I_ReflectionModel;
namespace MeshData {
struct Material;
}

class C_PathIntegrator {
public:
	using T_Alloc = Utils::Allocation::C_StdStackAllocator<I_ReflectionModel, 2048>;
	C_PathIntegrator(const C_RayTraceScene& scene);
	// main API of this class, allows to use of custom sampler
	[[nodiscard]] Colours::T_Colour TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd);

private:

	[[nodiscard]] Colours::T_Colour Li_LightSampling(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd, T_Alloc* alloc = nullptr);
	[[nodiscard]] Colours::T_Colour Li_Direct(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd, T_Alloc* alloc = nullptr);
	[[nodiscard]] Colours::T_Colour Li_PathTrace(Physics::Primitives::S_Ray ray, I_Sampler& rnd, T_Alloc* alloc = nullptr);

	template <class deleter = std::default_delete<Renderer::I_ReflectionModel>, class T_Alloct = void>
	[[nodiscard]] std::unique_ptr<I_ReflectionModel, deleter> GetReflectionModel(const MeshData::Material* material, Colours::T_Colour& colour, T_Alloct* alloc = nullptr) const;

	const C_RayTraceScene& m_Scene;
};
} // namespace GLEngine::Renderer