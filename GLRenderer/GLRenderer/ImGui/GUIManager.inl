#pragma once


namespace GLEngine::GLRenderer::ImGui {

//=================================================================================
template<class T, class ... Args>
std::reference_wrapper<GUI::Menu::C_MenuItem>
	GLEngine::GLRenderer::ImGui::C_GUIManager::CreateMenuItem(Args&&... args)
{
	static_assert(std::is_base_of_v<GUI::Menu::C_MenuItem, T>, "Can only create menu items derived classes");
	auto& it = m_MenuItems.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
	return *it;
}
}