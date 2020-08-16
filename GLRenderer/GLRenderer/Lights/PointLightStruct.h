#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Utils/Padding.h>

namespace GLEngine::GLRenderer {
struct S_PointLight : public Shaders::I_GLSLStruct<S_PointLight>
{
	S_PointLight();
	glm::vec3 m_Position;
	Utils::Padding<float> gap0;
	glm::vec3 m_Color;
	float m_Intensity;

	Shaders::C_StructDescriptor GetDescriptionImpl() const;
	std::string GetNameImpl() const;
};
}