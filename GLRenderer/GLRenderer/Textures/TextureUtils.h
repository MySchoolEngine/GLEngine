#pragma once

#include <Renderer/Textures/TextureStorage.h>

namespace GLEngine::GLRenderer::Textures {

inline GLenum GetFormat(Renderer::T_Channels channels)
{
	using namespace GLEngine::Renderer;
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha})
	{
		return GL_RGBA;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None})
	{
		return GL_RGB;
	}
	if (channels == T_Channels{E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::Alpha})
	{
		return GL_BGRA;
	}
	if (channels == T_Channels{E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::None})
	{
		return GL_BGR;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::None, E_TextureChannel::None})
	{
		return GL_RG;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::None, E_TextureChannel::None, E_TextureChannel::None})
	{
		return GL_RED;
	}
	return GL_RGBA;
}

} // namespace GLEngine::GLRenderer::Textures
