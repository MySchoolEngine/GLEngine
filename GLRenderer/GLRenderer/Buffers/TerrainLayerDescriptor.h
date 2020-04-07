#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Utils/Padding.h>

namespace GLEngine::GLRenderer {
struct S_TerrainLayerDescriptor : public Shaders::I_GLSLStruct
{
	glm::vec3 m_Color;
	float m_ThicknessFactor;
	float m_Granularity;
	Utils::Padding<float> gap0;
	Utils::Padding<float> gap1;
	Utils::Padding<float> gap2;

	virtual Shaders::C_StructDescriptor GetDescription() const override;
	virtual std::string GetName() const override;
};
}