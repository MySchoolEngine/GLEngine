#pragma once

#include <Renderer/Definitions.h>
#include <Renderer/Descriptors/BufferDescriptor.h>

#include <Utils/Hashing.h>

namespace GLEngine::Renderer {
enum class E_ComputeBindingKind : std::uint8_t {
	Buffer,
	Image,
	SampledImage,
};

struct ComputeBindingDescriptor {
	uint32_t			 binding;
	E_ComputeBindingKind kind;
	E_BufferType		 bufferType = E_BufferType::ShaderStorage; // ShaderStorage for both in/out in v1
};
struct ComputePipelineDescriptor {
	std::string							  shader; // pipeline XML name, same convention as PipelineDescriptor::shader
	std::vector<ComputeBindingDescriptor> bindings;
};
} // namespace GLEngine::Renderer

template <> struct std::hash<GLEngine::Renderer::ComputeBindingDescriptor> {
	std::size_t operator()(const GLEngine::Renderer::ComputeBindingDescriptor& s) const noexcept
	{
		auto h1 = std::hash<uint32_t>{}(s.binding);
		h1		= Utils::HashCombine(h1, s.kind);
		h1		= Utils::HashCombine(h1, s.bufferType);
		return h1;
	}
};

template <> struct std::hash<GLEngine::Renderer::ComputePipelineDescriptor> {
	std::size_t operator()(const GLEngine::Renderer::ComputePipelineDescriptor& s) const noexcept
	{
		auto h1 = std::hash<std::string>{}(s.shader);
		h1		= Utils::HashVector(h1, s.bindings);
		return h1;
	}
};