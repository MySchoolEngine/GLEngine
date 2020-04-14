#pragma once

namespace GLEngine::GLRenderer::GUI {
namespace Menu {
	class C_MenuItem;
}
class C_Window;
}

namespace GLEngine::GLRenderer::ImGui {
class C_GUIManager
{
public:
	C_GUIManager();
	~C_GUIManager();

	GUID CreateGUIWindow(const std::string& name);
	void AddCustomWindow(GUI::C_Window* window);

	// modal windows coming

	GUI::C_Window* GetWindow(GUID guid) const;

	void DestroyWindow(GUID guid);
	void OnUpdate();

	template<class T, class ... Args>
	std::reference_wrapper<GUI::Menu::C_MenuItem> CreateMenuItem(Args&&... args);
private:
	std::unordered_map<GUID, GUI::C_Window*>						m_Windwos;
	std::vector<std::unique_ptr<GUI::Menu::C_MenuItem>>	m_MenuItems;
};

}

#include <GLRenderer/ImGui/GUIManager.inl>

