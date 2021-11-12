#include <GUIStdafx.h>

#include <GUI/ReflectionGUI.h>

#include <imgui.h>

namespace GLEngine::GUI {

//=================================================================================
void DrawSlider(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;

	::ImGui::InputFloat3(GetMetadataMember<UI::Vec3::Name>(prop).c_str(), (float*)(prop.get_value(obj).convert<glm::vec3*>()));
}

void DrawCheckbox(rttr::instance& obj, const rttr::property& prop)
{
	using namespace ::Utils::Reflection;
	
	::ImGui::Checkbox(GetMetadataMember<UI::Checkbox::Name>(prop).c_str(), (prop.get_value(obj).convert<bool*>()));
}
//=================================================================================
void DrawAllPropertyGUI(rttr::instance& obj)
{
	using namespace ::Utils::Reflection;
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
		DrawSlider(obj, prop);
	}
}

}


