#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/Lights/AreaLight.h>

#include <Physics/Primitives/Frustum.h>

#include <Utils/Parsing/ColorParsing.h>
#include <Utils/Parsing/MatrixParse.h>

namespace GLEngine::Renderer {

//=================================================================================
C_AreaLight::C_AreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
	, m_WidthSlider(10.f, 0.1f, 10.f, "Width")
	, m_HeightSlider(10.f, 0.1f, 10.f, "Height")
	, m_DiffuseColor("Diffuse colour", glm::vec3(1.f))
	, m_SpecularColor("Spec colour", glm::vec3(1.f))
{
}

//=================================================================================
C_AreaLight::~C_AreaLight() = default;

//=================================================================================
Physics::Primitives::C_Frustum C_AreaLight::GetShadingFrustum() const
{
	auto						   transformMatrix = GetComponentModelMatrix();
	Physics::Primitives::C_Frustum ret(transformMatrix[3], GetUpVector(), GetNormal(), 0.1f, 50.f, 1.0f, 0.f);
	return ret;
}

//=================================================================================
Physics::Primitives::S_AABB C_AreaLight::GetAABB() const
{
	Physics::Primitives::S_AABB aabb;

	const auto dirY = GetUpVector();
	const auto dirX = glm::cross(GetNormal(), dirY);

	const auto width  = std::sqrt(GetWidth() / 2.0f);
	const auto height = std::sqrt(GetHeight() / 2.0f);

	aabb.Add(+dirY * height + dirX * width);
	aabb.Add(+dirY * height - dirX * width);
	aabb.Add(-dirY * height + dirX * width);
	aabb.Add(-dirY * height - dirX * width);

	return aabb;
}

//=================================================================================
void C_AreaLight::DebugDrawGUI(GUI::C_GUIManager* guiMGR /*= nullptr*/)
{
	m_WidthSlider.Draw();
	m_HeightSlider.Draw();
	m_DiffuseColor.Draw();
	m_SpecularColor.Draw();
}

//=================================================================================
std::string_view C_AreaLight::GetDebugComponentName() const
{
	return "Area light";
}

//=================================================================================
bool C_AreaLight::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
glm::vec3 C_AreaLight::DiffuseColour() const
{
	return m_DiffuseColor.GetValue();
}

//=================================================================================
glm::vec3 C_AreaLight::SpecularColour() const
{
	return m_SpecularColor.GetValue();
}

//=================================================================================
glm::vec3 C_AreaLight::GetNormal() const
{
	return GetComponentModelMatrix() * glm::vec4(0, 0, -1, 1);
}

//=================================================================================
glm::vec3 C_AreaLight::GetUpVector() const
{
	return GetComponentModelMatrix() * glm::vec4(0, 1, 0, 1);
}

//=================================================================================
void C_AreaLight::DebugDraw(Renderer::I_DebugDraw* dd) const
{
	const auto upVector = GetUpVector();
	const auto normal	= GetNormal();
	const auto dirX		= glm::cross(normal, upVector);
	const auto width	= std::sqrt(GetWidth() / 2.0f);
	const auto height	= std::sqrt(GetHeight() / 2.0f);

	auto transformMatrix = GetComponentModelMatrix();

	const auto Pos = glm::vec3(transformMatrix[3]);

	dd->DrawLine(Pos, Pos + normal, glm::vec3(1.f, 1.f, 0.f));

	dd->DrawLine(Pos + upVector * height + dirX * width, Pos + upVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));
	dd->DrawLine(Pos - upVector * height + dirX * width, Pos - upVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));

	dd->DrawPoint(Pos + upVector * height + dirX * width, glm::vec3(0, 1, 0));

	dd->DrawLine(Pos - upVector * height + dirX * width, Pos + upVector * height + dirX * width, glm::vec3(1.f, 1.f, 0.f));
	dd->DrawLine(Pos - upVector * height - dirX * width, Pos + upVector * height - dirX * width, glm::vec3(1.f, 1.f, 0.f));
}

//=================================================================================
[[nodiscard]] std::shared_ptr<Entity::I_Component> C_AreaLightCompBuilder::Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner)
{
	auto areaLight = std::make_shared<Renderer::C_AreaLight>(owner);
	areaLight->SetComponentMatrix(Utils::Parsing::C_MatrixParser::ParseTransformation(node));


	if (const auto widthAttr = node.attribute("width"))
	{
		areaLight->m_WidthSlider = widthAttr.as_float();
	}

	if (const auto heightAttr = node.attribute("height"))
	{
		areaLight->m_WidthSlider = heightAttr.as_float();
	}

	if (node.child("DiffuseColor"))
	{
		areaLight->m_DiffuseColor = Utils::Parsing::C_ColorParser::ParseColorRGB(node, "DiffuseColor");
	}

	if (node.child("SpecularColor"))
	{
		areaLight->m_SpecularColor = Utils::Parsing::C_ColorParser::ParseColorRGB(node, "SpecularColor");
	}

	return areaLight;
}

} // namespace GLEngine::Renderer