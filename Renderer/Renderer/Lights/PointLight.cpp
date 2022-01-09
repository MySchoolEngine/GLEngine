#include <RendererStdafx.h>

#include <Renderer/Lights/PointLight.h>
#include <Renderer/Mesh/Scene.h>

#include <Physics/Primitives/Frustum.h>

#include <Utils/Parsing/ColorParsing.h>
#include <Utils/Parsing/MatrixParse.h>

#include <rttr/registration.h>

RTTR_REGISTRATION
{
	rttr::registration::class_<GLEngine::Renderer::C_PointLight>("C_PointLight")
		.constructor<std::shared_ptr<GLEngine::Entity::I_Entity>>()
		.property_readonly("Intensity", &GLEngine::Renderer::C_PointLight::GetIntensity)
		.property_readonly("Color", &GLEngine::Renderer::C_PointLight::GetColor);
}

namespace GLEngine::Renderer {

//=================================================================================
// C_PointLight
//=================================================================================
C_PointLight::C_PointLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
	, m_Intensity(1.f, 0.f, 100.f, "Intensity")
	, m_Color("Color", Colours::white)
{
}

//=================================================================================
C_PointLight::C_PointLight(std::shared_ptr<Entity::I_Entity> owner, const MeshData::Light& def)
	: Renderer::I_Light(owner)
	, m_Intensity(1.f, 0.f, 100.f, "Intensity")
	, m_Color("Color", def.m_Color)
{
}

//=================================================================================
C_PointLight::~C_PointLight() = default;

//=================================================================================
Physics::Primitives::S_AABB C_PointLight::GetAABB() const
{
	Physics::Primitives::S_AABB ret;
	ret.Add(GetPosition());
	return ret;
}

//=================================================================================
glm::vec3 C_PointLight::GetPosition() const
{
	return glm::vec3(GetComponentModelMatrix()[3]);
}

//=================================================================================
float C_PointLight::GetIntensity() const
{
	return m_Intensity;
}

//=================================================================================
Colours::T_Colour C_PointLight::GetColor() const
{
	return m_Color.GetValue();
}

//=================================================================================
Physics::Primitives::C_Frustum C_PointLight::GetShadingFrustum() const
{
	return Physics::Primitives::C_Frustum(GetPosition(), GetPosition(), GetPosition(), 1.f, 1.f, 1.f, 1.f);
}

//=================================================================================
std::string_view C_PointLight::GetDebugComponentName() const
{
	return "Point light";
}

//=================================================================================
bool C_PointLight::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
void C_PointLight::DebugDrawGUI()
{
	m_Intensity.Draw();
	m_Color.Draw();
}

//=================================================================================
std::shared_ptr<Entity::I_Component> C_PointLightCompBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	auto pointLight = std::make_shared<Renderer::C_PointLight>(owner);
	pointLight->SetComponentMatrix(Utils::Parsing::C_MatrixParser::ParseTransformation(node));

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

} // namespace GLEngine::Renderer