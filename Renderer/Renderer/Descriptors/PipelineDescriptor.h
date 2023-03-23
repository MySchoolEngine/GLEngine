#pragma once

#include <Renderer/Definitions.h>

namespace GLEngine::Renderer {

struct PipelineDescriptor {
public:
	E_RenderPrimitives primitiveType;
	bool			   wireframe = false;
};
} // namespace GLEngine::Renderer
