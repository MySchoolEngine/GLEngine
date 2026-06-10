#pragma once

#include <imgui.h>

#include <string>
#include <vector>

namespace GLEngine::GUI {

template <typename T>
concept TabbedViewTab = requires(T t) {
	{ t.m_TabLabel } -> std::convertible_to<std::string>;
	{ t.m_bModified } -> std::convertible_to<bool>;
};

/**
 * Composable tab-bar component. Owns a vector of tabs and handles all ImGui
 * tab-bar rendering, active-tab tracking, and close-button lifecycle.
 *
 * TabT must expose:
 *   std::string m_TabLabel
 *   bool        m_bModified
 */
template <TabbedViewTab TabT>
class C_TabbedView {
public:
	// Renders the tab bar.
	// drawContent(TabT&) — called for the active tab's body.
	// onClose(TabT&)     — called before a closed tab is erased (use for resource cleanup).
	template <typename DrawFn, typename CloseFn>
	void Draw(std::string_view barId, DrawFn&& drawContent, CloseFn&& onClose, ImGuiTabBarFlags flags = ImGuiTabBarFlags_AutoSelectNewTabs) const
	{
		if (ImGui::BeginTabBar(barId.data(), flags))
		{
			for (unsigned int i = 0; i < m_Tabs.size(); ++i)
			{
				auto&			  tab	= m_Tabs[i];
				const std::string label = tab.m_TabLabel + (tab.m_bModified ? " *" : "") + "##tab" + std::to_string(i);
				bool			  open	= true;

				if (ImGui::BeginTabItem(label.c_str(), &open))
				{
					m_ActiveTabIndex = i;
					drawContent(tab);
					ImGui::EndTabItem();
				}

				if (!open)
				{
					onClose(tab);
					m_Tabs.erase(m_Tabs.begin() + i);
					if (m_ActiveTabIndex >= m_Tabs.size())
						m_ActiveTabIndex = static_cast<unsigned int>(m_Tabs.size()) > 0 ? static_cast<unsigned int>(m_Tabs.size()) - 1 : 0;
					--i;
				}
			}
			ImGui::EndTabBar();
		}
	}

	[[nodiscard]] bool HasTabs() const { return !m_Tabs.empty(); }

	TabT&		ActiveTab() { return m_Tabs[m_ActiveTabIndex]; }
	const TabT& ActiveTab() const { return m_Tabs[m_ActiveTabIndex]; }

	// Constructs a new tab in-place and returns a reference to it.
	// Note: invalidates all existing references (vector may reallocate).
	template <typename... Args>
	TabT& EmplaceTab(Args&&... args)
	{
		return m_Tabs.emplace_back(std::forward<Args>(args)...);
	}

	// Switches to the first tab satisfying pred. Returns true if found.
	template <typename Pred>
	bool TrySwitchTo(Pred&& pred)
	{
		for (unsigned int i = 0; i < m_Tabs.size(); ++i)
		{
			if (pred(m_Tabs[i]))
			{
				m_ActiveTabIndex = i;
				return true;
			}
		}
		return false;
	}

	mutable std::vector<TabT> m_Tabs;

private:
	mutable unsigned int m_ActiveTabIndex = 0;
};

} // namespace GLEngine::GUI
