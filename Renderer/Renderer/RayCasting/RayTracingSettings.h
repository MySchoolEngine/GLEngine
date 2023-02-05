#pragma once

#include <Utils/Allocation/StdStackAllocator.h>

namespace GLEngine::Renderer {
class I_ReflectionModel;
struct RayTracingSettings final {
public:
	using T_ReflAlloc = Utils::Allocation::C_StdStackAllocator<I_ReflectionModel, 2048>;
	using T_ReflPtr = std::unique_ptr<I_ReflectionModel, T_ReflAlloc::delete_policy<>>;
};
}

