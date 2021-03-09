#pragma once

#include <GLRendererStdafx.h>

namespace GLEngine::GLRenderer {
inline std::uint8_t GetNumberOfChannels(GLint pixelFormat)
{
	switch (pixelFormat)
	{
	case GL_RED:
		return 1;
	case GL_RG:
		return 2;
	case GL_RGB:
		return 3;
	case GL_RGBA:
		return 4;
	case GL_RGBA16F:
		return 4;
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown pixel format: {}", pixelFormat);
		return 0;
	}
}

} // namespace GLEngine::GLRenderer
