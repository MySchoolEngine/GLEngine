#pragma once

#include <string>


namespace GLEngine::Renderer {
enum class E_TextureFormat
{
};

enum class E_TextureType
{
	TEXTUE_2D
};

struct TextureDescriptor {
	std::string		name;
	std::size_t		widht;
	std::size_t		height;
	E_TextureFormat format;
	E_TextureType	type;
};
} // namespace GLEngine::Renderer
