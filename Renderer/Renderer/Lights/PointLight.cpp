#include <RendererStdafx.h>

#include <Renderer/Lights/PointLight.h>
#include <Renderer/Mesh/Scene.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/Primitives/Frustum.h>

#include <Utils/Parsing/ColorParsing.h>

#include <rttr/registration.h>

#pragma region registration
// clang-format off
RTTR_REGISTRATION
{
	using namespace Utils::Reflection;
	using namespace GLEngine::Renderer;

	rttr::registration::class_<C_PointLight>("C_PointLight")
		.constructor<std::shared_ptr<GLEngine::Entity::I_Entity>>()
		.constructor<>()
		.property("Intensity", &C_PointLight::m_Intensity)
  		  (
			  rttr::policy::prop::as_reference_wrapper,
			  RegisterMetaclass<MetaGUI::Slider>(),
			  RegisterMetamember<UI::Slider::Name>("Intensity:"),
			  RegisterMetamember<UI::Slider::Min>(0.f),
			  RegisterMetamember<UI::Slider::Max>(100.0f)
		  )
		.property("Color", &C_PointLight::m_Color)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Colour>(),
			RegisterMetamember<UI::Colour::Name>("Colour:")
		);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_PointLight>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_PointLight> ptr, bool& ok) -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}
// clang-format on
#pragma endregion registration

namespace GLEngine::Renderer {

//=================================================================================
// C_PointLight
//=================================================================================
C_PointLight::C_PointLight(const std::shared_ptr<Entity::I_Entity>& owner)
	: Renderer::I_Light(owner)
	, m_Intensity(1.f)
	, m_Color(Colours::white)
{
}

//=================================================================================
C_PointLight::C_PointLight(const std::shared_ptr<Entity::I_Entity>& owner, const MeshData::Light& def)
	: Renderer::I_Light(owner)
	, m_Intensity(1.f)
	, m_Color(def.m_Color)
{
}

//=================================================================================
C_PointLight::C_PointLight()
	: Renderer::I_Light(nullptr)
	, m_Intensity(1.f)
	, m_Color(Colours::white)
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
void C_PointLight::DebugDrawGUI()
{
	rttr::instance obj(*this);
	GUI::DrawAllPropertyGUI(obj);
}

} // namespace GLEngine::Renderer