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

	for (const auto& type : baseType.get_derived_classes()) {
		if (type.get_constructors().empty() == false)
		{
			m_Selected = type.get_name().data();
		}
	}
	GLE_ASSERT(m_Selected.empty() == false, "No derived class to select");
}

//=================================================================================
bool C_TypeSelector::Draw() const
{
	bool	   changed	= false;
	const auto baseType = rttr::type::get_by_name(m_BaseTypeName);

	if (::ImGui::BeginCombo(m_Name.c_str(), m_Selected.c_str()))
	{
		for (const auto& type : baseType.get_derived_classes())
		{
			if (type.get_constructors().empty()) continue;
			bool is_selected = type.get_name() == m_Selected;
			if (ImGui::Selectable(type.get_name().data(), is_selected))
			{
				if (type.get_name().data() != m_Selected) {
					m_Selected = type.get_name().data();
					changed	   = true;
				}
			}
			if (is_selected)
			{
				::ImGui::SetItemDefaultFocus();
			}
		}
		::ImGui::EndCombo();
	}
	return changed;
}

} // namespace GLEngine::GUI::Input
