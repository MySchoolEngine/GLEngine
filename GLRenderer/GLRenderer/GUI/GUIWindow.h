#pragma once

#include <GLRenderer/GUI/GUIPart.h>

#include <Core/GUID.h>

namespace GLEngine::GLRenderer::GUI {

class C_Window : public I_GUIPart {
public:
	using T_GUIPartRef = std::reference_wrapper<I_GUIPart>;
public:
	C_Window(GUID guid, const std::string& name);
	virtual ~C_Window() = default;

	virtual void Draw() const;
	void SetVisible(bool enable = true);
	bool IsVisible() const;
	GUID AddComponent(T_GUIPartRef component);
	I_GUIPart* GetComponent(GUID guid) const;

	GUID GetGuid() const { return m_GUID; }
		
protected:
	mutable bool m_IsVisible;
	std::string m_Name;
	GUID m_GUID;
private:
	std::map<GUID, T_GUIPartRef> m_Components;
};

}