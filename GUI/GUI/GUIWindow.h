#pragma once

#include <GUI/GUIPart.h>
#include <GUI/Menu/Menu.h>

#include <Core/GUID.h>

namespace GLEngine::GUI {

class GUI_API_EXPORT C_Window : public I_GUIPart {
public:
	using T_GUIPartRef = std::reference_wrapper<I_GUIPart>;
	using T_GUIMenu = std::reference_wrapper<Menu::C_Menu>;
public:
	C_Window(GUID guid, const std::string& name);
	virtual ~C_Window() = default;

	virtual void Draw() const override;
	void SetVisible(bool enable = true);
	bool IsVisible() const;
	GUID AddComponent(T_GUIPartRef component);
	I_GUIPart* GetComponent(GUID guid) const;

	GUID GetGuid() const { return m_GUID; }
		
	GUID AddMenu(T_GUIMenu menuItem);
protected:
	mutable bool m_IsVisible;
	std::string m_Name;
	GUID m_GUID;
private:
	std::map<GUID, T_GUIPartRef>	m_Components;
	std::map<GUID, T_GUIMenu> m_Menus;
};

}