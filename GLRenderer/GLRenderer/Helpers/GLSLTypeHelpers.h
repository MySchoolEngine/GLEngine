#pragma once 

#include <GLRendererStdafx.h>

namespace GLEngine::GLRenderer {

//=================================================================================
template<class T>
struct T_TypeToGLSL;

template<> struct T_TypeToGLSL<bool>					{ static constexpr std::string_view value = "bool"; };

template<> struct T_TypeToGLSL<float>					{ static constexpr std::string_view value = "float"; };
template<> struct T_TypeToGLSL<double>				{ static constexpr std::string_view value = "double"; };

template<> struct T_TypeToGLSL<int>						{ static constexpr std::string_view value = "int"; };
template<> struct T_TypeToGLSL<unsigned int>	{ static constexpr std::string_view value = "uint"; };

template<> struct T_TypeToGLSL<glm::vec1>			{ static constexpr std::string_view value = "float"; };
template<> struct T_TypeToGLSL<glm::vec2>			{ static constexpr std::string_view value = "vec2"; };
template<> struct T_TypeToGLSL<glm::vec3>			{ static constexpr std::string_view value = "vec3"; };
template<> struct T_TypeToGLSL<glm::vec4>			{ static constexpr std::string_view value = "vec4"; };

template<> struct T_TypeToGLSL<glm::ivec1>		{ static constexpr std::string_view value = "int"; };
template<> struct T_TypeToGLSL<glm::ivec2>		{ static constexpr std::string_view value = "ivec2"; };
template<> struct T_TypeToGLSL<glm::ivec3>		{ static constexpr std::string_view value = "ivec3"; };
template<> struct T_TypeToGLSL<glm::ivec4>		{ static constexpr std::string_view value = "ivec4"; };

template<> struct T_TypeToGLSL<glm::uvec1>		{ static constexpr std::string_view value = "uint"; };
template<> struct T_TypeToGLSL<glm::uvec2>		{ static constexpr std::string_view value = "uvec2"; };
template<> struct T_TypeToGLSL<glm::uvec3>		{ static constexpr std::string_view value = "uvec3"; };
template<> struct T_TypeToGLSL<glm::uvec4>		{ static constexpr std::string_view value = "uvec4"; };

template<> struct T_TypeToGLSL<glm::bvec1>		{ static constexpr std::string_view value = "bool"; };
template<> struct T_TypeToGLSL<glm::bvec2>		{ static constexpr std::string_view value = "bvec2"; };
template<> struct T_TypeToGLSL<glm::bvec3>		{ static constexpr std::string_view value = "bvec3"; };
template<> struct T_TypeToGLSL<glm::bvec4>		{ static constexpr std::string_view value = "bvec4"; };

template<> struct T_TypeToGLSL<glm::dvec1>		{ static constexpr std::string_view value = "double"; };
template<> struct T_TypeToGLSL<glm::dvec2>		{ static constexpr std::string_view value = "dvec2"; };
template<> struct T_TypeToGLSL<glm::dvec3>		{ static constexpr std::string_view value = "dvec3"; };
template<> struct T_TypeToGLSL<glm::dvec4>		{ static constexpr std::string_view value = "dvec4"; };

template<> struct T_TypeToGLSL<glm::mat2>			{ static constexpr std::string_view value = "mat2"; };
template<> struct T_TypeToGLSL<glm::mat3>			{ static constexpr std::string_view value = "mat3"; };
template<> struct T_TypeToGLSL<glm::mat4>			{ static constexpr std::string_view value = "mat4"; };

template<> struct T_TypeToGLSL<glm::mat2x3>		{ static constexpr std::string_view value = "mat2x3"; };
template<> struct T_TypeToGLSL<glm::mat2x4>		{ static constexpr std::string_view value = "mat2x4"; };	

template<> struct T_TypeToGLSL<glm::mat3x2>		{ static constexpr std::string_view value = "mat3x2"; };
template<> struct T_TypeToGLSL<glm::mat3x4>		{ static constexpr std::string_view value = "mat3x4"; };

template<> struct T_TypeToGLSL<glm::mat4x2>		{ static constexpr std::string_view value = "mat4x2"; };
template<> struct T_TypeToGLSL<glm::mat4x3>		{ static constexpr std::string_view value = "mat4x3"; };

template<> struct T_TypeToGLSL<std::uint64_t>	{ static constexpr std::string_view value = "sampler2D"; };

template<class T>
constexpr auto T_TypeToGLSL_v = T_TypeToGLSL<T>::value;

}