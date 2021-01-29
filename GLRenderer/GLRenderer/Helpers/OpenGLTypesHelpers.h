#pragma once

#include <GLRendererStdafx.h>

#include <Renderer/Textures/TextureStorage.h>

#include <Core/CoreMacros.h>

namespace GLEngine::GLRenderer {

//=================================================================================
template <class T> struct T_TypeToGL;

template <> struct T_TypeToGL<std::uint8_t> {
	static constexpr GLenum value = GL_UNSIGNED_BYTE;
};
template <> struct T_TypeToGL<char> {
	static constexpr GLenum value = GL_BYTE;
};

template <> struct T_TypeToGL<float> {
	static constexpr GLenum value = GL_FLOAT;
};
template <> struct T_TypeToGL<double> {
	static constexpr GLenum value = GL_DOUBLE;
};

template <> struct T_TypeToGL<int> {
	static constexpr GLenum value = GL_INT;
};
template <> struct T_TypeToGL<unsigned int> {
	static constexpr GLenum value = GL_UNSIGNED_INT;
};

template <> struct T_TypeToGL<short> {
	static constexpr GLenum value = GL_SHORT;
};
template <> struct T_TypeToGL<unsigned short> {
	static constexpr GLenum value = GL_UNSIGNED_SHORT;
};

template <> struct T_TypeToGL<glm::vec1> {
	static constexpr GLenum value = GL_FLOAT;
};
template <> struct T_TypeToGL<glm::vec2> {
	static constexpr GLenum value = GL_FLOAT;
};
template <> struct T_TypeToGL<glm::vec3> {
	static constexpr GLenum value = GL_FLOAT;
};
template <> struct T_TypeToGL<glm::vec4> {
	static constexpr GLenum value = GL_FLOAT;
};

template <> struct T_TypeToGL<glm::ivec1> {
	static constexpr GLenum value = GL_INT;
};
template <> struct T_TypeToGL<glm::ivec2> {
	static constexpr GLenum value = GL_INT;
};
template <> struct T_TypeToGL<glm::ivec3> {
	static constexpr GLenum value = GL_INT;
};
template <> struct T_TypeToGL<glm::ivec4> {
	static constexpr GLenum value = GL_INT;
};

//=================================================================================
[[nodiscard]] inline GLenum GetUnderlyingType(const Renderer::I_TextureViewStorage* tex)
{
	if (tex->GetStorageType() == Renderer::E_TextureTypes::Floating)
	{
		return GL_FLOAT;
	}
	if (tex->GetStorageType() == Renderer::E_TextureTypes::IntegralNormalized)
	{
		return GL_UNSIGNED_BYTE;
	}
	if (tex->GetStorageType() == Renderer::E_TextureTypes::Integral)
	{
		return GL_BYTE;
	}
	GLE_ASSERT(false, "Unsupported type");

	return GL_BYTE;
}

//=================================================================================
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
template <GLenum type> struct T_GLTypeIsIntegral : public std::false_type {
};

template <> struct T_GLTypeIsIntegral<GL_BYTE> : public std::true_type {
};
template <> struct T_GLTypeIsIntegral<GL_UNSIGNED_BYTE> : public std::true_type {
};
template <> struct T_GLTypeIsIntegral<GL_SHORT> : public std::true_type {
};
template <> struct T_GLTypeIsIntegral<GL_UNSIGNED_SHORT> : public std::true_type {
};
template <> struct T_GLTypeIsIntegral<GL_INT> : public std::true_type {
};
template <> struct T_GLTypeIsIntegral<GL_UNSIGNED_INT> : public std::true_type {
};

template <GLenum type> inline constexpr bool T_GLTypeIsIntegral_v = T_GLTypeIsIntegral<type>::value;

//=================================================================================
template <GLenum type> struct T_GLTypeIsDouble : public std::false_type {
};

template <> struct T_GLTypeIsDouble<GL_DOUBLE> : public std::true_type {
};

template <GLenum type> inline constexpr bool T_GLTypeIsDouble_v = T_GLTypeIsDouble<type>::value;

//=================================================================================
template <class T> struct T_GLNumComponenets;

template <> struct T_GLNumComponenets<unsigned char> {
	static constexpr std::size_t value = 1;
};
template <> struct T_GLNumComponenets<char> {
	static constexpr std::size_t value = 1;
};

template <> struct T_GLNumComponenets<float> {
	static constexpr std::size_t value = 1;
};
template <> struct T_GLNumComponenets<double> {
	static constexpr std::size_t value = 1;
};

template <> struct T_GLNumComponenets<int> {
	static constexpr std::size_t value = 1;
};
template <> struct T_GLNumComponenets<unsigned int> {
	static constexpr std::size_t value = 1;
};

template <> struct T_GLNumComponenets<short> {
	static constexpr std::size_t value = 1;
};
template <> struct T_GLNumComponenets<unsigned short> {
	static constexpr std::size_t value = 1;
};

template <> struct T_GLNumComponenets<glm::vec1> {
	static constexpr std::size_t value = 1;
};
template <> struct T_GLNumComponenets<glm::vec2> {
	static constexpr std::size_t value = 2;
};
template <> struct T_GLNumComponenets<glm::vec3> {
	static constexpr std::size_t value = 3;
};
template <> struct T_GLNumComponenets<glm::vec4> {
	static constexpr std::size_t value = 4;
};

template <> struct T_GLNumComponenets<glm::ivec1> {
	static constexpr std::size_t value = 1;
};
template <> struct T_GLNumComponenets<glm::ivec2> {
	static constexpr std::size_t value = 2;
};
template <> struct T_GLNumComponenets<glm::ivec3> {
	static constexpr std::size_t value = 3;
};
template <> struct T_GLNumComponenets<glm::ivec4> {
	static constexpr std::size_t value = 4;
};

template <class type> inline constexpr std::size_t T_GLNumComponenets_v = T_GLNumComponenets<type>::value;

//=================================================================================
enum class E_FramebufferTarget
{
	Framebuffer,
	Read,
	Draw,
};

template <E_FramebufferTarget framebuffer> struct T_FramebufferTarget;

template <> struct T_FramebufferTarget<E_FramebufferTarget::Framebuffer> {
	static constexpr GLenum value = GL_FRAMEBUFFER;
};
template <> struct T_FramebufferTarget<E_FramebufferTarget::Read> {
	static constexpr GLenum value = GL_READ_FRAMEBUFFER;
};
template <> struct T_FramebufferTarget<E_FramebufferTarget::Draw> {
	static constexpr GLenum value = GL_DRAW_FRAMEBUFFER;
};

//=================================================================================
// Wrap functions
enum class E_WrapFunction
{
	ClampToEdge,
	ClampToBorder,
	MirroredRepeat,
	Repeat,
	// MirrorClampToEdge,
};

constexpr GLenum WrapFunctionToEnum(const E_WrapFunction wrap)
{
	switch (wrap)
	{
	case E_WrapFunction::ClampToEdge:
		return GL_CLAMP_TO_EDGE;
	case E_WrapFunction::ClampToBorder:
		return GL_CLAMP_TO_BORDER;
	case E_WrapFunction::MirroredRepeat:
		return GL_MIRRORED_REPEAT;
	case E_WrapFunction::Repeat:
		return GL_REPEAT;
	}
	return GL_INVALID_VALUE;
}

template <E_WrapFunction wrapFunction> struct T_WrapFunction {
	static constexpr GLenum value = WrapFunctionToEnum(wrapFunction);
};

//=================================================================================
// MinMag filters
enum class E_OpenGLFilter : char
{
	Linear,
	Nearest,
	NearestMipMapNearest,
	LinearMipMapNearest,
	LinearMipMapLinear,
	NearestMipMapLinear,
};

constexpr GLenum MinMagFilterToEnum(const E_OpenGLFilter filter)
{
	switch (filter)
	{
	case E_OpenGLFilter::Linear:
		return GL_LINEAR;
	case E_OpenGLFilter::Nearest:
		return GL_NEAREST;
	case E_OpenGLFilter::NearestMipMapNearest:
		return GL_NEAREST_MIPMAP_NEAREST;
	case E_OpenGLFilter::LinearMipMapNearest:
		return GL_LINEAR_MIPMAP_NEAREST;
	case E_OpenGLFilter::LinearMipMapLinear:
		return GL_LINEAR_MIPMAP_LINEAR;
	case E_OpenGLFilter::NearestMipMapLinear:
		return GL_NEAREST_MIPMAP_LINEAR;
	}
	return GL_INVALID_VALUE;
}

//=================================================================================
// Access rights
enum class E_OpenGLAccess : char
{
	Read,
	Write,
	ReadWrite,
};

constexpr GLenum AccesRightsToEnum(const E_OpenGLAccess access)
{
	switch (access)
	{
	case E_OpenGLAccess::Read:
		return GL_READ_ONLY;
		break;
	case E_OpenGLAccess::Write:
		return GL_WRITE_ONLY;
		break;
	case E_OpenGLAccess::ReadWrite:
		return GL_READ_WRITE;
		break;
	}
	return GL_INVALID_VALUE;
}
} // namespace GLEngine::GLRenderer