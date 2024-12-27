#pragma once

#include <GLRendererStdafx.h>

#include <GLRenderer/Helpers/TextureHelpers.h>

#include <Renderer/Descriptors/PipelineDescriptor.h>

namespace GLEngine::GLRenderer {
inline void SetBlendFunction(const unsigned int bufIndex, const Renderer::BlendingDescriptor& desc)
{
	if (!desc.enable)
	{
		// we should disable blending
		return;
	}
	glBlendEquationi(bufIndex, GetBlendFunction(desc.blendColorFunction));
	glBlendFuncSeparatei(bufIndex, GetBlendFactor(desc.srcColorFactor), GetBlendFactor(desc.dstColorFactor), GetBlendFactor(desc.srcAlphaFactor),
						 GetBlendFactor(desc.dstColorFactor));
}
} // namespace GLEngine::GLRenderer
