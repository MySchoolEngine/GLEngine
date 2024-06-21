#include <GUIStdafx.h>

#include <GUI/ReflectionGUI.h>

#include <imgui.h>

namespace GLEngine::GUI {

//=================================================================================
void DrawText(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	::ImGui::Text(prop.get_value(obj).convert<std::string*>()->c_str());
}

//=================================================================================
bool DrawVec3(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::InputFloat3(GetMetadataMember<UI::Vec3::Name>(prop).c_str(), (float*)(prop.get_value(obj).convert<glm::vec3*>()));
}

//=================================================================================
bool DrawCheckbox(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::Checkbox(GetMetadataMember<UI::Checkbox::Name>(prop).c_str(), (prop.get_value(obj).convert<bool*>()));
}

//=================================================================================
bool DrawSlider(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::SliderFloat(GetMetadataMember<UI::Slider::Name>(prop).c_str(), (prop.get_value(obj).convert<float*>()), GetMetadataMember<UI::Slider::Min>(prop),
								GetMetadataMember<UI::Slider::Max>(prop));
}

//=================================================================================
bool DrawSliderInt(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::SliderInt(GetMetadataMember<UI::SliderInt::Name>(prop).c_str(), (prop.get_value(obj).convert<int*>()), GetMetadataMember<UI::SliderInt::Min>(prop),
							  GetMetadataMember<UI::SliderInt::Max>(prop));
}

//=================================================================================
bool DrawAngle(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::SliderAngle(GetMetadataMember<UI::Angle::Name>(prop).c_str(), (prop.get_value(obj).convert<float*>()), GetMetadataMember<UI::Angle::Min>(prop),
								GetMetadataMember<UI::Angle::Max>(prop));
}

//=================================================================================
bool DrawColour(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	return ::ImGui::ColorEdit3(GetMetadataMember<UI::Colour::Name>(prop).c_str(), (float*)(prop.get_value(obj).convert<glm::vec3*>()));
}

//=================================================================================
std::vector<rttr::property> DrawAllPropertyGUI(rttr::instance& obj)
{
	using namespace ::Utils::Reflection;
	std::vector<rttr::property> changedVals;

	if (UI::IsTypeUIMetaClass<MetaGUI::CustomGUIWidget>(obj))
	{
		// custom widgets
		GetMetadataMember<UI::CustomGUIWidget::DrawFunction>(obj)(obj);
		return {};
	}

	rttr::type type = obj.get_type().get_raw_type();
	if (type.get_derived_classes().empty() == false)
		type = obj.get_derived_type();

	std::map<std::string, std::vector<rttr::property>> collapsableHeaders;
	for (auto& prop : type.get_properties())
	{
		if (HasMetadataMember<MetaGUIInfo::CollapsableGroup>(prop))
		{
			collapsableHeaders[GetMetadataMember<MetaGUIInfo::CollapsableGroup>(prop)].emplace_back(prop);
		}
		else
		{
			if (DrawPropertyGUI(obj, prop))
			{
				changedVals.emplace_back(prop);
			}
		}
	}
	for (auto& collapsableHeader : collapsableHeaders)
	{
		if (::ImGui::CollapsingHeader(collapsableHeader.first.c_str()))
		{
			for (auto& prop : collapsableHeader.second)
			{
				if (DrawPropertyGUI(obj, prop))
				{
					changedVals.emplace_back(prop);
				}
			}
		}
	}
	return changedVals;
}

//=================================================================================
bool DrawPropertyGUI(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;
	if (UI::IsUIMetaclass<MetaGUI::Vec3>(prop))
	{
		return DrawVec3(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Checkbox>(prop))
	{
		return DrawCheckbox(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Colour>(prop))
	{
		return DrawColour(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Slider>(prop))
	{
		return DrawSlider(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::SliderInt>(prop))
	{
		return DrawSliderInt(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Angle>(prop))
	{
		return DrawAngle(obj, prop);
	}
	else if (UI::IsUIMetaclass<MetaGUI::Text>(prop))
	{
		DrawText(obj, prop);
		return false;
	}
	else
	{
		rttr::instance ins{prop.get_value(obj)};
		return GUI::DrawAllPropertyGUI(ins).empty() == false; // this will not return changed properties
	}
}

} // namespace GLEngine::GUI
