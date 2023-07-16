#pragma once

#include <Renderer/Definitions.h>
#include <Renderer/Descriptors/TextureDescriptor.h>

#include <Utils/Hashing.h>

#include <glm/gtx/hash.hpp>

#include <type_traits>

// ===================================================================
// Changes to this file implies need for regeneration of the PSO cache
// ===================================================================

namespace GLEngine::Renderer {

struct AttributeDescriptor {
public:
	uint32_t		 binding; // buffer from which to choose
	E_ShaderDataType type;
	// stride calculated from other inputs
};

struct BlendingDescriptor {
	bool			enable = false;
	E_BlendFunction blendColorFunction;
	E_BlendFactor	srcColorFactor;
	E_BlendFactor	dstColorFactor;
	E_BlendFunction blendAlphaFunction;
	E_BlendFactor	srcAlphaFactor;
	E_BlendFactor	dstAlphaFactor;
	T_ChannelsBits	writemask = E_TextureChannel::Red | E_TextureChannel::Green | E_TextureChannel::Blue | E_TextureChannel::Alpha; // todo  glColorMaski
};

struct PipelineDescriptor {
public:
	E_RenderPrimitives				 primitiveType;
	bool							 wireframe = false;
	std::vector<BlendingDescriptor>	 blending;
	glm::vec4						 blendConstant;
	uint32_t						 bindingCount = 0;
	std::vector<AttributeDescriptor> vertexInput;
	std::vector<AttributeDescriptor> instanceInput;
	std::string						 shader;

	// this will go to the attachment one day
	E_TextureFormat colorAttachementFormat;
};

} // namespace GLEngine::Renderer

template <> struct std::hash<GLEngine::Renderer::AttributeDescriptor> {
	std::size_t operator()(const GLEngine::Renderer::AttributeDescriptor& s) const noexcept
	{
		const auto h1 = std::hash<uint32_t>{}(s.binding);
		return Utils::HashCombine(h1, s.type);
	}
};

template <> struct std::hash<GLEngine::Renderer::BlendingDescriptor> {
	std::size_t operator()(const GLEngine::Renderer::BlendingDescriptor& s) const noexcept
	{
		auto h1 = std::hash<bool>{}(s.enable);
		h1		= Utils::HashCombine(h1, s.blendColorFunction);
		h1		= Utils::HashCombine(h1, s.srcColorFactor);
		h1		= Utils::HashCombine(h1, s.srcColorFactor);
		h1		= Utils::HashCombine(h1, s.dstColorFactor);
		h1		= Utils::HashCombine(h1, s.srcAlphaFactor);
		h1		= Utils::HashCombine(h1, s.dstAlphaFactor);
		h1		= Utils::HashCombine(h1, s.writemask);
		return h1;
	}
};

template <> struct std::hash<GLEngine::Renderer::PipelineDescriptor> {
	std::size_t operator()(const GLEngine::Renderer::PipelineDescriptor& s) const noexcept
	{
		auto h1 = std::hash<GLEngine::Renderer::E_RenderPrimitives>{}(s.primitiveType);
		h1		= Utils::HashCombine(h1, s.wireframe);
		h1		= Utils::HashVector(h1, s.blending);
		h1		= Utils::HashCombine(h1, s.blendConstant);
		h1		= Utils::HashCombine(h1, s.bindingCount);
		h1		= Utils::HashVector(h1, s.vertexInput);
		h1		= Utils::HashVector(h1, s.instanceInput);
		return h1;
	}
};
