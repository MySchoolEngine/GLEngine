#include <GLRendererStdafx.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine::GLRenderer {

struct FormatDescriptor {
	bool		 Compressed : 1;
	bool		 Intagral : 1;
	GLenum		 glUnderlyingType;
	GLenum		 glInternalFormat;
	GLenum		 glExternalFormat;
	std::uint8_t numComponents;
};

// clang-format off
static FormatDescriptor const TextureFormatDesc[] = {
	{false, false,GL_FLOAT, GL_RGBA32F,	GL_RGBA,4},							// E_TextureFormat::RGBA32f
	{false, false,GL_FLOAT, GL_RGB32F,	GL_RGB, 3},							// E_TextureFormat::RGB32f
	{false, false,GL_FLOAT, GL_RG32F,	GL_RG,	2},							// E_TextureFormat::RG32f
	{false, false,GL_FLOAT, GL_R32F,	GL_RED, 1},							// E_TextureFormat::R32f

	{false, false,GL_FLOAT, GL_RGBA16F,	GL_RGBA,4},							// E_TextureFormat::RGBA16f
	{false, false,GL_FLOAT, GL_RGB16F,	GL_RGB, 3},							// E_TextureFormat::RGB16f
	{false, false,GL_FLOAT, GL_RG16F,	GL_RG,  2},							// E_TextureFormat::RG16f
	{false, false,GL_FLOAT, GL_R16F,	GL_RED, 1},							// E_TextureFormat::R16f

	{false, true,GL_INT, GL_RGBA32I,GL_RGBA_INTEGER,4},						// E_TextureFormat::RGBA32i
	{false, true,GL_INT, GL_RGB32I,	GL_RGB_INTEGER, 3},						// E_TextureFormat::RGB32i
	{false, true,GL_INT, GL_RG32I,	GL_RG_INTEGER,  2},						// E_TextureFormat::RG32i
	{false, true,GL_INT, GL_R32I,	GL_RED_INTEGER, 1},						// E_TextureFormat::R32i

	{false, true,GL_SHORT, GL_RGBA16I, GL_RGBA_INTEGER,4},					// E_TextureFormat::RGBA16i
	{false, true,GL_SHORT, GL_RGB16I, GL_RGB_INTEGER,  3},					// E_TextureFormat::RGB16i
	{false, true,GL_SHORT, GL_RG16I, GL_RG_INTEGER,	   2},					// E_TextureFormat::RG16i
	{false, true,GL_SHORT, GL_R16I, GL_RED_INTEGER,	   1},					// E_TextureFormat::R16i

	{false, true,GL_BYTE, GL_RGBA8I,	GL_RGBA_INTEGER, 4},				// E_TextureFormat::RGBA8i
	{false, true,GL_BYTE, GL_RGB8I,		GL_RGB_INTEGER,  3},				// E_TextureFormat::RGB8i
	{false, true,GL_BYTE, GL_RG8I,		GL_RG_INTEGER,   2},				// E_TextureFormat::RG8i
	{false, true,GL_BYTE, GL_R8I,		GL_RED_INTEGER,  1},				// E_TextureFormat::R8i

	{false, true, GL_BYTE, GL_SRGB8_ALPHA8, GL_RGBA_INTEGER,	4},			// E_TextureFormat::RGBA8isrgb
	{false, true, GL_BYTE, GL_SRGB8,		GL_BGR_INTEGER,		4},			// E_TextureFormat::BGRA8isrgb		// Not sure! :(
	{false, true, GL_BYTE, GL_SRGB8,		GL_RGB_INTEGER,		3},			// E_TextureFormat::RGB8isrgb
	{false, true, GL_BYTE, GL_SRGB8,		GL_RGB_INTEGER,		2},			// E_TextureFormat::RG8isrgb		// Not sure! :(
	{false, true, GL_BYTE, GL_SRGB8,		GL_RGB_INTEGER,		1},			// E_TextureFormat::R8isrgb			// Not sure! :(

	{false, false, GL_UNSIGNED_INT_24_8, GL_DEPTH_STENCIL,		GL_NONE, 2},// E_TextureFormat::D24S8
	{false, false, GL_FLOAT,			 GL_DEPTH_COMPONENT32F, GL_NONE, 1},// E_TextureFormat::D32f
	{false, false, GL_UNSIGNED_INT_24_8, GL_DEPTH_COMPONENT24,  GL_NONE, 1},// E_TextureFormat::D24,
	{false, false, GL_HALF_FLOAT,		 GL_DEPTH_COMPONENT16,	GL_NONE, 1},// E_TextureFormat::D16
};
// clang-format on

constexpr std::size_t Count = sizeof(TextureFormatDesc) / sizeof(FormatDescriptor);
static_assert(Count == static_cast<std::underlying_type_t<Renderer::E_TextureFormat>>(Renderer::E_TextureFormat::Count), "Each format has to have descriptor");

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

} // namespace GLEngine::GLRenderer