#include <GUIStdafx.h>

#include <GUI/ReflectionGUI.h>

#include <imgui.h>

namespace GLEngine::GUI {

//=================================================================================
void DrawVec3(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	::ImGui::InputFloat3(GetMetadataMember<UI::Vec3::Name>(prop).c_str(), (float*)(prop.get_value(obj).convert<glm::vec3*>()));
}

//=================================================================================
void DrawCheckbox(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;
	
	::ImGui::Checkbox(GetMetadataMember<UI::Checkbox::Name>(prop).c_str(), (prop.get_value(obj).convert<bool*>()));
}

//=================================================================================
void DrawSlider(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	::ImGui::SliderFloat(GetMetadataMember<UI::Slider::Name>(prop).c_str(),
		(prop.get_value(obj).convert<float*>()),
		GetMetadataMember<UI::Slider::Min>(prop),
		GetMetadataMember<UI::Slider::Max>(prop));
}

//=================================================================================
void DrawColour(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	::ImGui::ColorEdit3(GetMetadataMember<UI::Colour::Name>(prop).c_str(), (float*)(prop.get_value(obj).convert<glm::vec3*>()));
}

//=================================================================================
void DrawAllPropertyGUI(rttr::instance& obj)
{
	using namespace ::Utils::Reflection;

	if (UI::IsTypeUIMetaClass<MetaGUI::CustomGUIWidget>(obj))
	{
		// custom widgets
		GetMetadataMember<UI::CustomGUIWidget::DrawFunction>(obj)(obj);
		return;
	}

	for (const auto& prop : obj.get_type().get_properties())
	{
		DrawPropertyGUI(obj, prop);
	}
}

//=================================================================================
GUI_API_EXPORT void DrawPropertyGUI(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;
	if (UI::IsUIMetaclass<MetaGUI::Vec3>(prop))
	{
		DrawVec3(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Checkbox>(prop))
	{
		DrawCheckbox(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Colour>(prop))
	{
		DrawColour(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Slider>(prop))
	{
		DrawSlider(obj, prop);
	}
}

}


