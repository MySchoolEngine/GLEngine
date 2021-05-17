#pragma once

#include <imgui.h>

namespace GLEngine::GUI::Input {

//=================================================================================
template <class Key>
C_Select<Key>::C_Select(const std::string_view name, const Key& defaultKey, const std::string_view label)
	: m_Name(name)
{
	AddValue(defaultKey, label);
	m_Selected = m_Previous = m_Values.begin();
}

//=================================================================================
template <class Key> bool C_Select<Key>::Changed() const
{
	return m_Selected != m_Previous;
}

//=================================================================================
template <class Key> const Key& C_Select<Key>::GetSelectedValue() const
{
	return m_Selected->first;
}

//=================================================================================
template <class Key> void C_Select<Key>::AddValue(const Key& key, const std::string_view label)
{
	// todo save curr and prev?
	m_Values.emplace(key, label);
}

//=================================================================================
template <class Key> void C_Select<Key>::Draw() const
{
	m_Previous = m_Selected;
	if (::ImGui::BeginCombo(m_Name.c_str(), m_Selected->second.c_str()))
	{
		for (auto it = m_Values.begin(); it != m_Values.end(); it++)
		{
			bool is_selected = it == m_Selected;
			if (ImGui::Selectable(it->second.c_str(), is_selected))
			{
				m_Selected = it;
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
