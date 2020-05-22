#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Utils/Padding.h>

namespace GLEngine::GLRenderer {
struct S_AreaLight : public Shaders::I_GLSLStruct<S_AreaLight>
{
	S_AreaLight();
	glm::mat4											m_LightMat;
	glm::vec3											m_Pos;
	Utils::Padding<float>					gap0;
	glm::vec3											m_Normal;
	float													m_Intensity;
	glm::vec3											m_Color;
	int														m_ShadowMap;
	glm::vec3											m_SpecularColor;
	Utils::Padding<float>					gap1;
	glm::vec3											m_DirX;
	float													m_Width;
	glm::vec3											m_DirY;
	float													m_Height;

	Shaders::C_StructDescriptor GetDescriptionImpl() const;
	std::string GetNameImpl() const;
};
}