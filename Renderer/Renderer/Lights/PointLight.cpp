#include <RendererStdafx.h>

#include <Renderer/Lights/PointLight.h>
#include <Renderer/Mesh/Scene.h>

#include <Physics/Primitives/Frustum.h>

#include <Utils/Parsing/ColorParsing.h>
#include <Utils/Parsing/MatrixParse.h>

namespace GLEngine::Renderer {

//=================================================================================
I_PointLight::I_PointLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
{
	m_Transformation.SetEnabledTransforms(GUI::Input::C_Transformations::E_Transorms::Translate);
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
	, m_Intensity()
	, m_Color(1.f, 1.f, 1.f)
{
}

//=================================================================================
C_PointLight::C_PointLight(std::shared_ptr<Entity::I_Entity> owner, const MeshData::Light& def)
	: Renderer::I_PointLight(owner)
	, m_Intensity()
	, m_Color(def.m_Color)
{
}

//=================================================================================
C_PointLight::~C_PointLight() = default;

//=================================================================================
glm::vec3 C_PointLight::GetPosition() const
{
	const auto owner		 = GetOwner();
	auto	   ownerPosition = glm::vec3(0.f);
	if (!owner)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Point light without owner");
	}
	ownerPosition = owner->GetPosition();

	return ownerPosition + glm::vec3(GetComponentModelMatrix()[3]);
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