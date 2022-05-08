#include <GUIStdafx.h>

#include <GUI/Input/TypeSelector.h>

#include <rttr/type>

namespace GLEngine::GUI::Input {

//=================================================================================
C_TypeSelector::C_TypeSelector(const std::string_view name, const std::string& baseTypeName)
	: m_BaseTypeName(baseTypeName)
	, m_Name(name)
{
	const auto baseType		  = rttr::type::get_by_name(m_BaseTypeName);
	const auto derivedClasses = baseType.get_derived_classes();
	const auto autoselected	  = derivedClasses.begin();
	GLE_ASSERT(autoselected != derivedClasses.end(), "No derived class to select");
	m_Selected = (*autoselected).get_name().data();
}

//=================================================================================
void C_TypeSelector::Draw() const
{
	const auto baseType = rttr::type::get_by_name(m_BaseTypeName);

	if (::ImGui::BeginCombo(m_Name.c_str(), m_Selected.c_str()))
	{
		for (const auto& type : baseType.get_derived_classes())
		{
			bool is_selected = type.get_name() == m_Selected;
			if (ImGui::Selectable(type.get_name().data(), is_selected))
			{
				m_Selected = type.get_name().data();
			}
			if (is_selected)
			{
				::ImGui::SetItemDefaultFocus();
			}
		}
		::ImGui::EndCombo();
	}
}

} // namespace GLEngine::GUI::Input
