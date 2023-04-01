#pragma once

#include <Renderer/Definitions.h>

namespace GLEngine::Renderer {

struct AttributeDescriptor {
public:
	uint32_t		 binding; // buffer from which to choose
	E_ShaderDataType type;
	// stride calculated from other inputs
};

struct PipelineDescriptor {
public:
	E_RenderPrimitives				 primitiveType;
	bool							 wireframe = false;
	std::vector<AttributeDescriptor> vertexInput;
	std::vector<AttributeDescriptor> instanceInput;
};
} // namespace GLEngine::Renderer
