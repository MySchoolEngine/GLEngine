#pragma once

#include <Renderer/Textures/TextureDefinitions.h>

#include <string>

namespace GLEngine::Renderer {

enum class E_TextureType
{
	TEXTUE_2D
};

struct TextureDescriptor {
	std::string name;
	std::size_t width;
	std::size_t height;
	E_TextureType type;

	E_TextureFormat format;
};
} // namespace GLEngine::Renderer
