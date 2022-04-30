#pragma once

#include <Renderer/Textures/TextureStorage.h>

namespace GLEngine::GLRenderer::Textures {

inline GLenum GetFormat(Renderer::T_Channels channels, bool isIntegral)
{
	using namespace GLEngine::Renderer;
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha})
	{
		return isIntegral ? GL_RGBA_INTEGER : GL_RGBA;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None})
	{
		return isIntegral ? GL_RGB_INTEGER : GL_RGB;
	}
	if (channels == T_Channels{E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::Alpha})
	{
		return isIntegral ? GL_BGRA_INTEGER : GL_BGRA;
	}
	if (channels == T_Channels{E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::None})
	{
		return isIntegral ? GL_BGR_INTEGER : GL_BGR;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::None, E_TextureChannel::None})
	{
		return isIntegral ? GL_RG_INTEGER : GL_RG;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::None, E_TextureChannel::None, E_TextureChannel::None})
	{
		return isIntegral ? GL_RED_INTEGER : GL_RED;
	}
	return isIntegral ? GL_RGBA_INTEGER : GL_RGBA;
}

} // namespace GLEngine::GLRenderer::Textures
