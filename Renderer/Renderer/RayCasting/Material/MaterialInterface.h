#pragma once

#include <Renderer/RayCasting/RayTracingSettings.h>

namespace GLEngine::Renderer {
class C_RayIntersection;
class I_MaterialInterface {
public:
	virtual ~I_MaterialInterface() = default;

	[[nodiscard]] virtual RayTracingSettings::T_ReflPtr GetScatteringFunction(const C_RayIntersection& intersect, RayTracingSettings::T_ReflAlloc& alloc) const = 0;
};
} // namespace GLEngine::Renderer