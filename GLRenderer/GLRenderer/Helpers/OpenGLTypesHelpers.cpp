#include <GLRendererStdafx.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine::GLRenderer {

struct FormatDescriptor {
	bool Compressed : 1;
	bool   Intagral : 1;
	GLenum glUnderlyingType;
	GLenum glInternalFormat;
	GLenum glExternalFormat;
	std::uint8_t numComponents;
};

// using namespace Renderer;
static FormatDescriptor const TextureFormatDesc[] = {
	{false, false,GL_FLOAT, GL_RGBA32F,	GL_RGBA,4},
	{false, false,GL_FLOAT, GL_RGB32F,	GL_RGB, 3},
	{false, false,GL_FLOAT, GL_RG32F,	GL_RG,	2},
	{false, false,GL_FLOAT, GL_R32F,	GL_RED, 1},

	{false, false,GL_FLOAT, GL_RGBA16F,	GL_RGBA,4},
	{false, false,GL_FLOAT, GL_RGB16F,	GL_RGB, 3},
	{false, false,GL_FLOAT, GL_RG16F,	GL_RG,  2},
	{false, false,GL_FLOAT, GL_R16F,	GL_RED, 1},

	{false, true,GL_INT, GL_RGBA32I,GL_RGBA_INTEGER,4},
	{false, true,GL_INT, GL_RGB32I,	GL_RGB_INTEGER, 3},
	{false, true,GL_INT, GL_RG32I,	GL_RG_INTEGER,  2},
	{false, true,GL_INT, GL_R32I,	GL_RED_INTEGER, 1},

	{false, true,GL_SHORT, GL_RGBA16I, GL_RGBA_INTEGER,4},
	{false, true,GL_SHORT, GL_RGB16I, GL_RGB_INTEGER,  3},
	{false, true,GL_SHORT, GL_RG16I, GL_RG_INTEGER,	   2},
	{false, true,GL_SHORT, GL_R16I, GL_RED_INTEGER,	   1},

	{false, true,GL_BYTE, GL_RGBA8I,	GL_RGBA_INTEGER, 4},
	{false, true,GL_BYTE, GL_RGB8I,		GL_RGB_INTEGER,  3},
	{false, true,GL_BYTE, GL_RG8I,		GL_RG_INTEGER,   2},
	{false, true,GL_BYTE, GL_R8I,		GL_RED_INTEGER,  1},

	{false, true, GL_BYTE, GL_SRGB8_ALPHA8, GL_RGBA_INTEGER,	4},
	{false, true, GL_BYTE, GL_SRGB8,		GL_BGR_INTEGER,		3}, // Not sure! :(
	{false, true, GL_BYTE, GL_SRGB8,		GL_RGB_INTEGER,		3}, // Not sure! :(
	{false, true, GL_BYTE, GL_SRGB8,		GL_RGB_INTEGER,		3}, // Not sure! :(
	{false, true, GL_BYTE, GL_SRGB8,		GL_RGB_INTEGER,		3}, // Not sure! :(

	{false, false, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL,		GL_NONE, 2},
	{false, false, GL_FLOAT,			 GL_DEPTH_COMPONENT32F, GL_NONE, 1},
	{false, false, GL_UNSIGNED_INT_24_8, GL_DEPTH_COMPONENT24,  GL_NONE, 1},
	{false, false, GL_HALF_FLOAT,		 GL_DEPTH_COMPONENT16,	GL_NONE, 1},
};

constexpr std::size_t Count = sizeof(TextureFormatDesc) / sizeof(FormatDescriptor);
static_assert(Count == static_cast<std::underlying_type_t<Renderer::E_TextureFormat>>(Renderer::E_TextureFormat::Count), 
  "Each format has to have descriptor");

//=================================================================================
inline constexpr const FormatDescriptor& GetFormatDescriptor(const Renderer::E_TextureFormat format)
{
	return TextureFormatDesc[static_cast<std::underlying_type_t<Renderer::E_TextureFormat>>(format)];
}

//=================================================================================
GLenum GetOpenGLFormat(const Renderer::E_TextureFormat format)
{
	return GetFormatDescriptor(format).glExternalFormat;
}

//=================================================================================
GLenum GetOpenGLInternalFormat(const Renderer::E_TextureFormat format)
{
	return GetFormatDescriptor(format).glInternalFormat;
}

//=================================================================================
GLenum OpenGLUnderlyingType(const Renderer::E_TextureFormat format)
{
	return GetFormatDescriptor(format).glUnderlyingType;
}

}