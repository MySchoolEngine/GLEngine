#pragma once

#include <GUI/GUIPart.h>
#include <GUI/Menu/Menu.h>

#include <Core/GUID.h>

namespace GLEngine::GUI {
class C_GUIManager;

/**
 * Class representing GUI window. Base for editors and editor utilities
 *
 * Default behaviour is to destroy window on hide.
 */
class GUI_API_EXPORT C_Window {
public:
	using T_GUIPartRef = std::reference_wrapper<I_GUIPart>;
	using T_GUIMenu	   = std::reference_wrapper<Menu::C_Menu>;

	C_Window(GUID guid, std::string name);
	virtual ~C_Window() = default;

	virtual void			 Update() {}
	virtual bool			 Draw(C_GUIManager& guiMgr) const;
	void					 SetVisible(bool enable = true);
	[[nodiscard]] bool		 IsVisible() const;
	GUID					 AddComponent(T_GUIPartRef component);
	[[nodiscard]] I_GUIPart* GetComponent(GUID guid) const;

	[[nodiscard]] GUID GetGuid() const { return m_GUID; }

	GUID AddMenu(T_GUIMenu menuItem);

	/**
	 * Called when requesting destruction. Can be overloaded to start deallocation of the resources.
	 */
	virtual void	   RequestDestroy() { m_WantToBeDestroyed = true; }
	[[nodiscard]] bool WantDestroy() const { return m_WantToBeDestroyed; }
	/**
	 * Can be derived in order to wait until all resources are freed. E.g. threads
	 * @return true if possible to destroy window
	 */
	[[nodiscard]] virtual bool CanDestroy() const { return true; }

protected:
	virtual void OnSetVisible() {}
	virtual void OnHide();
	virtual void DrawComponents() const;
	void		 DrawMenus() const;

	mutable bool m_IsVisible; // cant be bit field as it is being referenced inside
	mutable bool m_WantToBeDestroyed : 1;
	bool		 m_bDestroyOnHide : 1 = true;
	std::string	 m_Name;
	GUID		 m_GUID;

private:
	std::vector<std::pair<GUID, T_GUIPartRef>> m_Components;
	std::map<GUID, T_GUIMenu>				   m_Menus;
};

} // namespace GLEngine::GUI