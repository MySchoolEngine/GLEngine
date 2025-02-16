#pragma once

#include <Renderer/Definitions.h>

namespace GLEngine::Renderer {
enum class E_BufferType : std::uint8_t
{
	Vertex,
	Index,
	Uniform,
	DrawIndirect,
	DispatchIndirect,
	AtomicBuffer,
	ShaderStorage,
	TransformFeedback,
};

enum class E_ResourceUsage : std::uint8_t
{
	Immutable, // in Vulkan DST and DEVICE_LOCAL_BIT? TODO Investigate further
	Dynamic,   //
	Stream,
	Persistent, // Persistently mapped to CPU
};

struct BufferDescriptor {
public:
	uint32_t		size;
	E_BufferType	type;
	E_ResourceUsage usage;
	std::string		name = "";
};
} // namespace GLEngine::Renderer
