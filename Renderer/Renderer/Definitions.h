#pragma once

/**
 * This file contains all cross renderer definitions.
 */

#include <glm/glm.hpp>

namespace GLEngine::Renderer {
enum class E_ShaderStage
{
	Vertex,
	Fragment,
	Geometry,
	TesselationControl,
	TesselationEvaluation,
	Compute,
	RayGen,
};

enum class E_RenderPrimitives
{
	TriangleList,
	LineList,
	PointList
};

enum class E_ShaderDataType
{
	Float,
	Vec2,
	Vec3,
	Vec4,
};

template <class T> struct T_TypeShaderDataType;

template <> struct T_TypeShaderDataType<float>		{ static constexpr E_ShaderDataType value	= E_ShaderDataType::Float;};
template <> struct T_TypeShaderDataType<glm::vec2>	{ static constexpr E_ShaderDataType value	= E_ShaderDataType::Vec2;};
template <> struct T_TypeShaderDataType<glm::vec3>	{ static constexpr E_ShaderDataType value	= E_ShaderDataType::Vec3;};
template <> struct T_TypeShaderDataType<glm::vec4>	{ static constexpr E_ShaderDataType value	= E_ShaderDataType::Vec4;};

template <class T>
constexpr E_ShaderDataType T_TypeShaderDataType_v = T_TypeShaderDataType<T>::value;

inline uint32_t ShaderDataTypeSize(const E_ShaderDataType type)
{
	switch (type)
	{
	case E_ShaderDataType::Float:
		return sizeof(float);
	case E_ShaderDataType::Vec2:
		return sizeof(glm::vec2);
	case E_ShaderDataType::Vec3:
		return sizeof(glm::vec3);
	case E_ShaderDataType::Vec4:
		return sizeof(glm::vec4);
	default:
		return 0;
	}
}
} // namespace GLEngine::Renderer
