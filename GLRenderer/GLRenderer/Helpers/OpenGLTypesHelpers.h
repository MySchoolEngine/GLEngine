#pragma once 

#include <GLRendererStdafx.h>

namespace GLEngine {
namespace GLRenderer {

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
}
}