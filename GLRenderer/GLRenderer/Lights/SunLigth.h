#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Utils/Padding.h>

namespace GLEngine::GLRenderer {
struct S_SunLight : public Shaders::I_GLSLStruct<S_SunLight>
{
	S_SunLight();
	const glm::vec3& GetSunPosition() const { return m_SunPosition; }
	void SetSunPosition(const glm::vec3& val) { m_SunPosition = val; }

	Shaders::C_StructDescriptor GetDescriptionImpl() const;
	std::string GetNameImpl() const;

	glm::vec3	m_SunPosition;
	float		m_SunDiscMultiplier;
	glm::vec3	m_SunColor;
	float		m_AsymetricFactor; // mie scattering [0,1]
	glm::mat4	m_LightViewProjection;
	std::uint64_t m_SunShadowMap;
};
}