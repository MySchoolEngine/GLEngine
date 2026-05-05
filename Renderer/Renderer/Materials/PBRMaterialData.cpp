#include <RendererStdafx.h>

#include <Renderer/Materials/PBRMaterialData.h>

#include <GUI/ReflectionGUI.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_PBRMaterialData>("C_PBRMaterialData")
		.constructor<>()
		.constructor<std::string>()
		.property("Name", &C_PBRMaterialData::m_Name)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Text>(),
			RegisterMetamember<SerializationCls::NoSerialize>(true)
		)
		.property("Color", &C_PBRMaterialData::m_Color)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Colour>(),
			RegisterMetamember<UI::Colour::Name>("Colour:")
		)
		.property("Roughness", &C_PBRMaterialData::m_Roughness)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Roughness:"),
			RegisterMetamember<UI::Slider::Min>(0.f),
			RegisterMetamember<UI::Slider::Max>(1.0f)
		)
		.property("ColorMapRes", &C_PBRMaterialData::m_ColorMapRes)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Texture>(),
			RegisterMetamember<UI::Texture::Name>("Colour map"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<TextureResource>())
		)
		.property("NormalMapRes", &C_PBRMaterialData::m_NormalMapRes)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Texture>(),
			RegisterMetamember<UI::Texture::Name>("Normal map"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<TextureResource>())
		)
		.property("RoughnessMapRes", &C_PBRMaterialData::m_RoughnessMapRes)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Texture>(),
			RegisterMetamember<UI::Texture::Name>("Roughness map"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<TextureResource>())
		);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_PBRMaterialData>>();
}
// clang-format on

namespace GLEngine::Renderer {

//=================================================================================
C_PBRMaterialData::C_PBRMaterialData(const std::string& name)
	: m_Name(name)
{
}

//=================================================================================
bool C_PBRMaterialData::DrawGUI()
{
	rttr::instance obj(*this);
	return GUI::DrawAllPropertyGUI(obj).empty() == false;
}

} // namespace GLEngine::Renderer
