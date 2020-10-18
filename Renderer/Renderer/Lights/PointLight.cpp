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
Physics::Primitives::S_AABB I_PointLight::GetAABB() const
{
	Physics::Primitives::S_AABB ret;
	ret.Add(glm::vec3(0.0f, 0.0f, 0.0f));
	return ret;
}

//=================================================================================
I_PointLight::~I_PointLight() = default;

//=================================================================================
// C_PointLight
//=================================================================================
C_PointLight::C_PointLight(std::shared_ptr<Entity::I_Entity> owner) 
	: Renderer::I_PointLight(owner)
	, m_Intensity(1.f, 0.f, 100.f, "Intensity")
	, m_Color("Color", { 1.f, 1.f, 1.f })
{
}

//=================================================================================
C_PointLight::C_PointLight(std::shared_ptr<Entity::I_Entity> owner, const MeshData::Light& def)
	: Renderer::I_PointLight(owner)
	, m_Intensity(1.f, 0.f, 100.f, "Intensity")
	, m_Color("Color", def.m_Color)
{

}

//=================================================================================
C_PointLight::~C_PointLight() = default;

//=================================================================================
glm::vec3 C_PointLight::GetPosition() const
{
	const auto owner = GetOwner();
	auto ownerPosition = glm::vec3(0.f);
	if (!owner)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Point light without owner");
	}
	ownerPosition = owner->GetPosition();

	return ownerPosition + glm::vec3(m_ComponentMatrix[3]);
}

//=================================================================================
float C_PointLight::GetIntensity() const
{
	return m_Intensity;
}

//=================================================================================
glm::vec3 C_PointLight::GetColor() const
{
	return m_Color.GetValue();
}

//=================================================================================
Physics::Primitives::C_Frustum C_PointLight::GetShadingFrustum() const
{
	return Physics::Primitives::C_Frustum(GetPosition(), GetPosition(), GetPosition(), 1.f ,1.f, 1.f, 1.f);
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
	pointLight->m_ComponentMatrix = Utils::Parsing::C_MatrixParser::ParseTransformation(node);

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