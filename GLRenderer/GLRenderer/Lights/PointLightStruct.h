#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Utils/Padding.h>

namespace GLEngine::GLRenderer {
struct S_PointLight : public Shaders::I_GLSLStruct
{
	glm::vec3 m_Position;
	Utils::Padding<float> gap0;
	glm::vec3 m_Color;
	Utils::Padding<float> gap1;
	float m_Intensity;

	virtual Shaders::C_StructDescriptor GetDescription() const override;
	virtual std::string GetName() const override;
};
}