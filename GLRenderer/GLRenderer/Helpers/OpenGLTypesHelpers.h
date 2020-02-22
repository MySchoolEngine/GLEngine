#pragma once 

#include <GLRendererStdafx.h>

namespace GLEngine::GLRenderer {

//=================================================================================
template<class T>
struct T_TypeToGL;

template<> struct T_TypeToGL<unsigned char> {	static constexpr GLenum value = GL_UNSIGNED_BYTE; };
template<> struct T_TypeToGL<char>			{	static constexpr GLenum value = GL_BYTE; };

template<> struct T_TypeToGL<float>			{	static constexpr GLenum value = GL_FLOAT; };
template<> struct T_TypeToGL<double>		{	static constexpr GLenum value = GL_DOUBLE; };

template<> struct T_TypeToGL<int>			{	static constexpr GLenum value = GL_INT; };
template<> struct T_TypeToGL<unsigned int>	{	static constexpr GLenum value = GL_UNSIGNED_INT; };

template<> struct T_TypeToGL<short>			{	static constexpr GLenum value = GL_SHORT; };
template<> struct T_TypeToGL<unsigned short>{	static constexpr GLenum value = GL_UNSIGNED_SHORT; };

template<> struct T_TypeToGL<glm::vec1> { static constexpr GLenum value = GL_FLOAT; };
template<> struct T_TypeToGL<glm::vec2> { static constexpr GLenum value = GL_FLOAT; };
template<> struct T_TypeToGL<glm::vec3> { static constexpr GLenum value = GL_FLOAT; };
template<> struct T_TypeToGL<glm::vec4> { static constexpr GLenum value = GL_FLOAT; };

template<> struct T_TypeToGL<glm::ivec1> { static constexpr GLenum value = GL_INT; };
template<> struct T_TypeToGL<glm::ivec2> { static constexpr GLenum value = GL_INT; };
template<> struct T_TypeToGL<glm::ivec3> { static constexpr GLenum value = GL_INT; };
template<> struct T_TypeToGL<glm::ivec4> { static constexpr GLenum value = GL_INT; };

//=================================================================================
// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
template<GLenum type>
struct T_GLTypeIsIntegral : public std::false_type {};

template<> struct T_GLTypeIsIntegral<GL_BYTE> : public std::true_type {};
template<> struct T_GLTypeIsIntegral<GL_UNSIGNED_BYTE> : public std::true_type {};
template<> struct T_GLTypeIsIntegral<GL_SHORT> : public std::true_type {};
template<> struct T_GLTypeIsIntegral<GL_UNSIGNED_SHORT> : public std::true_type {};
template<> struct T_GLTypeIsIntegral<GL_INT> : public std::true_type {};
template<> struct T_GLTypeIsIntegral<GL_UNSIGNED_INT> : public std::true_type {};

template<GLenum type>
inline constexpr bool T_GLTypeIsIntegral_v = T_GLTypeIsIntegral<type>::value;

//=================================================================================
template<GLenum type>
struct T_GLTypeIsDouble : public std::false_type {};

template<> struct T_GLTypeIsDouble<GL_DOUBLE> : public std::true_type {};

template<GLenum type>
inline constexpr bool T_GLTypeIsDouble_v = T_GLTypeIsDouble<type>::value;

//=================================================================================
template<class T>
struct T_GLNumComponenets;

template<> struct T_GLNumComponenets<unsigned char> { static constexpr std::size_t value = 1; };
template<> struct T_GLNumComponenets<char>					{ static constexpr std::size_t value = 1; };

template<> struct T_GLNumComponenets<float>					{ static constexpr std::size_t value = 1; };
template<> struct T_GLNumComponenets<double>				{ static constexpr std::size_t value = 1; };

template<> struct T_GLNumComponenets<int>						{ static constexpr std::size_t value = 1; };
template<> struct T_GLNumComponenets<unsigned int>	{ static constexpr std::size_t value = 1; };

template<> struct T_GLNumComponenets<short>					{ static constexpr std::size_t value = 1; };
template<> struct T_GLNumComponenets<unsigned short>{ static constexpr std::size_t value = 1; };

template<> struct T_GLNumComponenets<glm::vec1>			{ static constexpr std::size_t value = 1; };
template<> struct T_GLNumComponenets<glm::vec2>			{ static constexpr std::size_t value = 2; };
template<> struct T_GLNumComponenets<glm::vec3>			{ static constexpr std::size_t value = 3; };
template<> struct T_GLNumComponenets<glm::vec4>			{ static constexpr std::size_t value = 4; };

template<> struct T_GLNumComponenets<glm::ivec1>		{ static constexpr std::size_t value = 1; };
template<> struct T_GLNumComponenets<glm::ivec2>		{ static constexpr std::size_t value = 2; };
template<> struct T_GLNumComponenets<glm::ivec3>		{ static constexpr std::size_t value = 3; };
template<> struct T_GLNumComponenets<glm::ivec4>		{ static constexpr std::size_t value = 4; };

template<class type>
inline constexpr std::size_t T_GLNumComponenets_v = T_GLNumComponenets<type>::value;

//=================================================================================
enum class E_FramebufferTarget
{
	Framebuffer,
	Read,
	Draw,
};

template<E_FramebufferTarget framebuffer>
struct T_FramebufferTarget;

template <> struct T_FramebufferTarget<E_FramebufferTarget::Framebuffer>	{ static constexpr GLenum value = GL_FRAMEBUFFER; };
template <> struct T_FramebufferTarget<E_FramebufferTarget::Read>					{ static constexpr GLenum value = GL_READ_FRAMEBUFFER; };
template <> struct T_FramebufferTarget<E_FramebufferTarget::Draw>					{ static constexpr GLenum value = GL_DRAW_FRAMEBUFFER; };

//=================================================================================
// Wrap functions
enum class E_WrapFunction
{
	ClampToEdge,
	ClampToBorder,
	MirroredRepeat,
	Repeat,
	//MirrorClampToEdge,
};

constexpr GLenum WrapFunctionToEnum(const E_WrapFunction wrap) {
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
}

template<E_WrapFunction wrapFunction>
struct T_WrapFunction
{
	static constexpr GLenum value = WrapFunctionToEnum(wrapFunction);
};
}