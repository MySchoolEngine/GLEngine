#pragma once

#include <Renderer/Definitions.h>
#include <Renderer/Descriptors/TextureDescriptor.h>


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
};
} // namespace GLEngine::Renderer
