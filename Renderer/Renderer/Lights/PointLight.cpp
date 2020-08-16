#include <RendererStdafx.h>

#include <Renderer/Lights/PointLight.h>

#include <Utils/Parsing/MatrixParse.h>
#include <Utils/Parsing/ColorParsing.h>

namespace GLEngine::Renderer {

//=================================================================================
I_PointLight::I_PointLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
{

}

//=================================================================================
I_PointLight::~I_PointLight() = default;

//=================================================================================
// C_PointLight
//=================================================================================
C_PointLight::C_PointLight(std::shared_ptr<Entity::I_Entity> owner) 
	: Renderer::I_PointLight(owner)
	, m_Intensity()
	, m_Offset(0.0f, 0.0f, 0.0f)
	, m_Color(1.f, 1.f, 1.f)
{
}

//=================================================================================
C_PointLight::C_PointLight(std::shared_ptr<Entity::I_Entity> owner, const MeshData::Light& def)
	: Renderer::I_PointLight(owner)
	, m_Intensity()
	, m_Offset(0.0f, 0.0f, 0.0f)
	, m_Color(def.m_Color)
{

}

//=================================================================================
C_PointLight::~C_PointLight() = default;

//=================================================================================
glm::vec3 C_PointLight::GetPosition() const
{
	auto owner = GetOwner();
	if (!owner)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Point light without owner");
		return glm::vec3(0.f);
	}

	return owner->GetPosition() + m_Offset;
}

//=================================================================================
float C_PointLight::GetIntensity() const
{
	return m_Intensity;
}

//=================================================================================
glm::vec3 C_PointLight::GetColor() const
{
	return m_Color;
}

//=================================================================================
Physics::Primitives::C_Frustum C_PointLight::GetShadingFrustum() const
{
	return Physics::Primitives::C_Frustum(GetPosition(), GetPosition(), GetPosition(), 1.f ,1.f, 1.f, 1.f);
}

//=================================================================================
std::shared_ptr<Entity::I_Component> C_PointLightCompBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	auto pointLight = std::make_shared<Renderer::C_PointLight>(owner);
	pointLight->m_Offset = Utils::Parsing::C_MatrixParser::ParseTransformation(node)[3];

	if (const auto intensityAttr = node.attribute("intensity"))
	{
		pointLight->m_Intensity = intensityAttr.as_float();
	}

	if (node.child("color"))
	{
		pointLight->m_Color = Utils::Parsing::C_ColorParser::ParseColorRGB(node);
	}

	return pointLight;
}

}