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

// values correspond to VkAccessFlagBits2
enum class E_AccessFlags : std::uint64_t {
	None				= 0,
	IndirectCommandRead = BIT(0),  // indirect arg fetch
	IndexBufferRead		= BIT(1),  // index consumption
	VertexAttributeRead = BIT(2),  // vertex consumption
	UniformBufferRead	= BIT(3),  // UBO read
	ShaderRead			= BIT(5),  // SSBO/UBO/texel reads from a shader
	ShaderWrite			= BIT(6),  // SSBO/storage writes from a shader
	TransferReadBit		= BIT(11), // copy dest / source
	TransferWriteBit	= BIT(12), // copy dest / source
	HostRead			= BIT(13), // CPU read of mapped memory
	HostWrite			= BIT(14), // CPU write of mapped memory
};

// values correspond to VkPipelineStageFlagBits2
enum class E_PipelineStage : std::uint16_t {
	TopOfPipe	  = BIT(0),
	BottomOfPipe  = BIT(13),
	DrawIndirect  = BIT(1),	 // reading a buffer as indirect dispatch/draw args (when compute writes indirect args)
	VertexInput	  = BIT(2),	 // consuming a compute-written buffer as vertex/index data
	VertexShader  = BIT(3),	 // consuming a compute-written buffer as vertex/index data
	ComputeShader = BIT(11), // the compute dispatch; both sides of a chained compute→compute barrier
	Transfer	  = BIT(12), // vkCmdCopy*/Fill/Update (staging upload, buffer copies)
	Host		  = BIT(14), // CPU access to mapped memory; dst side of a readback barrier
	AllCommands	  = BIT(16), // everything; correct but coarse, kills parallelism
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


//=================================================================================
template <> struct DULib::enable_BitField_operators<GLEngine::Renderer::E_AccessFlags> {
	static constexpr bool enable = true;
};
//=================================================================================
template <> struct DULib::enable_BitField_operators<GLEngine::Renderer::E_PipelineStage> {
	static constexpr bool enable = true;
};


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