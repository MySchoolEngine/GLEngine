#include <RendererStdafx.h>

#include <Renderer/DebugDraw.h>
#include <Renderer/Lights/AreaLight.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/Primitives/Frustum.h>

#include <Utils/Parsing/ColorParsing.h>
#include <Utils/Parsing/MatrixParse.h>

#include <glm/gtx/matrix_decompose.hpp>

#include <rttr/registration>

#pragma region registration
// clang-format off
RTTR_REGISTRATION
{
	using namespace Utils::Reflection;
	using namespace GLEngine::Renderer;
	
	rttr::registration::class_<C_AreaLight>("C_AreaLight")
		.constructor<std::shared_ptr<GLEngine::Entity::I_Entity>>()
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("Width", &C_AreaLight::m_Width)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Width:"),
				RegisterMetamember<UI::Slider::Min>(.1f),
				RegisterMetamember<UI::Slider::Max>(10.0f)
			)
		.property("Height", &C_AreaLight::m_Height)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Slider>(),
				RegisterMetamember<UI::Slider::Name>("Height:"),
				RegisterMetamember<UI::Slider::Min>(.1f),
				RegisterMetamember<UI::Slider::Max>(10.0f)
			)
		.property("DiffuseColour", &C_AreaLight::m_DiffuseColor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Diffuse colour:")
			)
		.property("SpecColour", &C_AreaLight::m_SpecularColor)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Colour>(),
				RegisterMetamember<UI::Colour::Name>("Spec colour:")
			);
	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_AreaLight>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_AreaLight> ptr, bool& ok) -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}
// clang-format on
#pragma endregion registration


namespace GLEngine::Renderer {

//=================================================================================
C_AreaLight::C_AreaLight(std::shared_ptr<Entity::I_Entity> owner)
	: Renderer::I_Light(owner)
	, m_Width(1.f)
	, m_Height(1.f)
	, m_DiffuseColor(Colours::white)
	, m_SpecularColor(Colours::white)
{
}

//=================================================================================
C_AreaLight::C_AreaLight()
	: Renderer::I_Light(nullptr)
	, m_Width(1.f)
	, m_Height(1.f)
	, m_DiffuseColor(Colours::white)
	, m_SpecularColor(Colours::white)
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
void C_AreaLight::DebugDrawGUI()
{
	rttr::instance obj(*this);
	GUI::DrawAllPropertyGUI(obj);
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
Colours::T_Colour C_AreaLight::DiffuseColour() const
{
	return m_DiffuseColor;
}

//=================================================================================
Colours::T_Colour C_AreaLight::SpecularColour() const
{
	return m_SpecularColor;
}

//=================================================================================
glm::vec3 C_AreaLight::GetNormal() const
{
	glm::mat4		transform(1.f);
	const glm::vec3 rotations = m_Transformation.GetRotation();
	transform				  = glm::rotate(transform, rotations.x, glm::vec3(1, 0, 0));
	transform				  = glm::rotate(transform, rotations.y, glm::vec3(0, 1, 0));
	transform				  = glm::rotate(transform, rotations.z, glm::vec3(0, 0, 1));
	return transform * glm::vec4(0, 0, -1, 1);
}

//=================================================================================
glm::vec3 C_AreaLight::GetUpVector() const
{
	glm::mat4		transform(1.f);
	const glm::vec3 rotations = m_Transformation.GetRotation();
	transform				  = glm::rotate(transform, rotations.x, glm::vec3(1, 0, 0));
	transform				  = glm::rotate(transform, rotations.y, glm::vec3(0, 1, 0));
	transform				  = glm::rotate(transform, rotations.z, glm::vec3(0, 0, 1));
	return transform * glm::vec4(0, 1, 0, 1);
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

	dd->DrawLine(Pos, Pos + normal, Colours::yellow);

	dd->DrawLine(Pos + upVector * height + dirX * width, Pos + upVector * height - dirX * width, Colours::yellow);
	dd->DrawLine(Pos - upVector * height + dirX * width, Pos - upVector * height - dirX * width, Colours::yellow);

	dd->DrawPoint(Pos + upVector * height + dirX * width, Colours::green);

	dd->DrawLine(Pos - upVector * height + dirX * width, Pos + upVector * height + dirX * width, Colours::yellow);
	dd->DrawLine(Pos - upVector * height - dirX * width, Pos + upVector * height - dirX * width, Colours::yellow);
}

} // namespace GLEngine::Renderer