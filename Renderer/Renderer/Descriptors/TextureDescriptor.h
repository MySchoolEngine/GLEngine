#pragma once

#include <Renderer/Textures/TextureDefinitions.h>

#include <string>

namespace GLEngine::Renderer {

enum class E_TextureType
{
	TEXTUE_2D,
	CUBE_MAP,
};

struct TextureDescriptor {

	std::string	  name;
	std::size_t	  width;
	std::size_t	  height;
	E_TextureType type;

	E_TextureFormat format;

	//!< Allows GPU device manager to stream in and out texture
	//!< set to false for e.g. GBuffer, shadow maps etc
	bool m_bStreamable  = true;

	std::uint8_t m_Levels	  = 1; //< Num mip maps
	std::uint8_t m_NumSamples = 1; //< Multisampled textures
};

struct SamplerDescriptor2D {

	// filtering
	Renderer::E_TextureFilter m_FilterMin;
	Renderer::E_TextureFilter m_FilterMag;

	// border
	glm::vec4				 m_BorderColor;
	Renderer::E_WrapFunction m_WrapS;
	Renderer::E_WrapFunction m_WrapT;
};
} // namespace GLEngine::Renderer
