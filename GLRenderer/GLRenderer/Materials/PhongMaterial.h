#pragma once

#include <GLRenderer/Shaders/Generation/ShaderStructs.h>

#include <Renderer/Materials/Material.h>

#include <Utils/Padding.h>

namespace GLEngine::GLRenderer::Material {
class C_PhongMaterial : public Shaders::I_GLSLStruct<C_PhongMaterial> {
public:
	std::uint64_t	m_ColorMap;
	std::uint64_t	m_NormalMap;
	std::uint64_t	m_RoughnessMap;
	Utils::Padding<float, 2> gap0;
	glm::vec3		m_ModelColor;
	float			m_Roughness;
	float			m_Shininess;
	int				m_UseNormalMap;
	Utils::Padding<float, 2> gap1;

	Shaders::C_StructDescriptor GetDescriptionImpl() const;
	std::string GetNameImpl() const;

	void Update(const Renderer::C_Material& material);
};
}
