#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RayCasting/RayTracingSettings.h>

#include <Physics/Primitives/Ray.h>

namespace GLEngine::Renderer {
class I_Sampler;
class C_RayTraceScene;
class I_ReflectionModel;
class C_RayIntersection;
namespace RayTracing {
class I_RayLight;
}
namespace MeshData {
struct Material;
}

class I_Integrator {
public:
	virtual ~I_Integrator() = default;

	[[nodiscard]] virtual Colours::T_Colour TraceRay(Physics::Primitives::S_Ray ray, I_Sampler& rnd) = 0;
};
} // namespace GLEngine::Renderer