#pragma once

#include <imgui.h>

#include <optional>
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
	// Renders the tab bar without save confirmation.
	// onClose(TabT&) — called before a closed tab is erased regardless of m_bModified.
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
					EraseTab(i);
					--i;
				}
			}
			ImGui::EndTabBar();
		}
	}

	// Renders the tab bar with save confirmation for modified tabs.
	// onSave(TabT&)    — called when user clicks Save; tab is closed only if m_bModified becomes false.
	// onDiscard(TabT&) — called for resource cleanup when user clicks Discard or tab is unmodified.
	template <typename DrawFn, typename SaveFn, typename DiscardFn>
	void Draw(std::string_view barId, DrawFn&& drawContent, SaveFn&& onSave, DiscardFn&& onDiscard, ImGuiTabBarFlags flags = ImGuiTabBarFlags_AutoSelectNewTabs) const
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

				if (!open && !m_PendingCloseIndex)
				{
					if (tab.m_bModified)
					{
						m_PendingCloseIndex = i;
						ImGui::OpenPopup("Save changes?##TabClose");
					}
					else
					{
						onDiscard(tab);
						EraseTab(i);
						--i;
					}
				}
			}
			ImGui::EndTabBar();
		}

		if (m_PendingCloseIndex)
		{
			if (ImGui::BeginPopupModal("Save changes?##TabClose", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
			{
				auto& pendingTab = m_Tabs[*m_PendingCloseIndex];
				ImGui::Text("Save changes to \"%s\"?", pendingTab.m_TabLabel.c_str());
				ImGui::Separator();

				if (ImGui::Button("Save", ImVec2(100, 0)))
				{
					onSave(pendingTab);
					if (!pendingTab.m_bModified)
						EraseTab(*m_PendingCloseIndex);
					m_PendingCloseIndex.reset();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Discard", ImVec2(100, 0)))
				{
					onDiscard(pendingTab);
					EraseTab(*m_PendingCloseIndex);
					m_PendingCloseIndex.reset();
					ImGui::CloseCurrentPopup();
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(100, 0)))
				{
					m_PendingCloseIndex.reset();
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
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

	mutable std::vector<TabT>			 m_Tabs;
	mutable std::optional<unsigned int>  m_PendingCloseIndex;

private:
	void EraseTab(unsigned int i) const
	{
		m_Tabs.erase(m_Tabs.begin() + i);
		if (m_ActiveTabIndex >= m_Tabs.size() && !m_Tabs.empty())
			m_ActiveTabIndex = static_cast<unsigned int>(m_Tabs.size()) - 1;
	}

	mutable unsigned int m_ActiveTabIndex = 0;
};

} // namespace GLEngine::GUI
