#pragma once

#include <GLRendererStdafx.h>

#include <Renderer/Textures/TextureDefinitions.h>
#include <Renderer/Descriptors/TextureDescriptor.h>

namespace GLEngine::GLRenderer {
inline GLint GetTextureType(const Renderer::E_TextureType e)
{
	switch (e)
	{
	case Renderer::E_TextureType::TEXTURE_2D:
		return GL_TEXTURE_2D;
	case Renderer::E_TextureType::CUBE_MAP:
		return GL_TEXTURE_CUBE_MAP;
	case Renderer::E_TextureType::TEXTURE_2D_ARRAY:
		return GL_TEXTURE_2D_ARRAY;
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown texture target: {}", e);
		return GL_TEXTURE_2D;
	}
}

//=================================================================================
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

//=================================================================================
inline GLenum GetBlendFunction(const Renderer::E_BlendFunction e)
{
	switch (e)
	{
	case Renderer::E_BlendFunction::Add:
		return GL_FUNC_ADD;
	case Renderer::E_BlendFunction::Subtract:
		return GL_FUNC_SUBTRACT;
	case Renderer::E_BlendFunction::ReverseSubtract:
		return GL_FUNC_REVERSE_SUBTRACT;
	case Renderer::E_BlendFunction::Min:
		return GL_MIN;
	case Renderer::E_BlendFunction::Max:
		return GL_MAX;
	default:
		GLE_ASSERT(false, "Unknown blend function");
		return GL_FUNC_ADD;
	}
}

//=================================================================================
inline GLenum GetBlendFactor(const Renderer::E_BlendFactor e)
{
	switch (e)
	{
	case Renderer::E_BlendFactor::Zero:
		return GL_ZERO;
	case Renderer::E_BlendFactor::One:
		return GL_ONE;
	case Renderer::E_BlendFactor::SourceColor:
		return GL_SRC_COLOR;
	case Renderer::E_BlendFactor::InvSourceColor:
		return GL_ONE_MINUS_SRC_COLOR;
	case Renderer::E_BlendFactor::SourceAlpha:
		return GL_SRC_ALPHA;
	case Renderer::E_BlendFactor::InvSourceAlpha:
		return GL_ONE_MINUS_SRC_ALPHA;
	case Renderer::E_BlendFactor::DestinationColor:
		return GL_DST_COLOR;
	case Renderer::E_BlendFactor::InvDestinationColor:
		return GL_ONE_MINUS_DST_COLOR;
	case Renderer::E_BlendFactor::DestinationAlpha:
		return GL_DST_ALPHA;
	case Renderer::E_BlendFactor::InvDestinationAlpha:
		return GL_ONE_MINUS_DST_ALPHA;
	case Renderer::E_BlendFactor::AlphaConstant:
		return GL_CONSTANT_COLOR;
	case Renderer::E_BlendFactor::InvAlphaConstant:
		return GL_ONE_MINUS_CONSTANT_COLOR;
	case Renderer::E_BlendFactor::ColorConstant:
		return GL_CONSTANT_ALPHA;
	case Renderer::E_BlendFactor::InvColorConstant:
		return GL_ONE_MINUS_CONSTANT_ALPHA;
	default:
		GLE_ASSERT(false, "Unknown blend factor");
		return GL_ONE;
	}
}

} // namespace GLEngine::GLRenderer
