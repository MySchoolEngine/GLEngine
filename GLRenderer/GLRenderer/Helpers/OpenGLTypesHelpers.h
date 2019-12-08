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