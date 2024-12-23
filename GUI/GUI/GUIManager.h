#pragma once

#include <GUI/GUIApi.h>

namespace GLEngine::GUI {
namespace Menu {
class C_MenuItem;
}
class C_Window;
} // namespace GLEngine::GUI

namespace GLEngine::GUI {
class GUI_API_EXPORT C_GUIManager {
public:
	C_GUIManager();
	~C_GUIManager();
	C_GUIManager(const C_GUIManager&)	= delete;
	C_GUIManager(C_GUIManager&&)		= delete;
	void operator=(const C_GUIManager&) = delete;
	void operator=(C_GUIManager&&)		= delete;

	GUID CreateGUIWindow(const std::string& name);
	void AddCustomWindow(C_Window* window);

	// modal windows coming

	[[nodiscard]] C_Window* GetWindow(GUID guid) const;

	void			   RequestDestroy();
	[[nodiscard]] bool CanBeDestroyed() const;

	void DestroyWindow(GUID guid);
	void OnUpdate();

	template <class T, class... Args> std::reference_wrapper<Menu::C_MenuItem> CreateMenuItem(Args&&... args);

private:
	void DestroyPossibleWindows();

	std::unordered_map<GUID, C_Window*>			   m_Windows;
	std::vector<std::unique_ptr<Menu::C_MenuItem>> m_MenuItems;
};

} // namespace GLEngine::GUI

#include <GUI/GUIManager.inl>
