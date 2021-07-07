#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Renderer/Colours.h>

#include <Utils/Padding.h>

namespace GLEngine::GLRenderer {
struct S_PointLight : public Shaders::I_GLSLStruct<S_PointLight> {
	S_PointLight();
	glm::vec3			  m_Position;
	Utils::Padding<float> gap0;
	Colours::T_Colour	  m_Color;
	float				  m_Intensity;

	Shaders::C_StructDescriptor GetDescriptionImpl() const;
	std::string					GetNameImpl() const;
};
} // namespace GLEngine::GLRenderer