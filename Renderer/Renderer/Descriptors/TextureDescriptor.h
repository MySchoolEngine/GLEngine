#pragma once

#include <Renderer/Textures/TextureDefinitions.h>

#include <string>

namespace GLEngine::Renderer {

enum class E_TextureType
{
	TEXTUE_2D
};

struct TextureDescriptor {
	std::string	  name;
	std::size_t	  width;
	std::size_t	  height;
	E_TextureType type;

	E_TextureFormat format;

	std::uint8_t m_Levels	  = 1; //< Num mip maps
	std::uint8_t m_NumSamples = 1; //< Multisampled textures
};
} // namespace GLEngine::Renderer
