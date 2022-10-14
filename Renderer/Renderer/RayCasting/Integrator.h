#pragma once

#include <Renderer/Colours.h>

#include <Physics/Primitives/Ray.h>

namespace GLEngine::Renderer {
class I_Sampler;
class C_RayTraceScene;
class I_ReflectionModel;
namespace MeshData {
struct Material;
}

class C_PathIntegrator {
public:
	C_PathIntegrator(const C_RayTraceScene& scene);
	// main API of this class, allows to use of custom sampler
	[[nodiscard]] Colours::T_Colour TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd);

private:
	[[nodiscard]] Colours::T_Colour Li_LightSampling(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd);
	[[nodiscard]] Colours::T_Colour Li_Direct(const Physics::Primitives::S_Ray& ray, I_Sampler& rnd);
	[[nodiscard]] Colours::T_Colour Li_PathTrace(Physics::Primitives::S_Ray ray, I_Sampler& rnd);

	[[nodiscard]] std::unique_ptr<I_ReflectionModel> GetReflectionModel(const MeshData::Material* material, Colours::T_Colour& colour) const;

	const C_RayTraceScene& m_Scene;
};
} // namespace GLEngine::Renderer