#pragma once

#include <Renderer/IDebugGUIComponent.h>

namespace GLEngine::GUI {

class C_DebugGUIComponent : public Renderer::I_DebugGUIComponent {
public:
	C_DebugGUIComponent(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_DebugGUIComponent() = default;

	//=====================================
	virtual void Toggle() override;
	virtual void PostUpdate() override;

	void SetTitle(const std::string& title);

	virtual void DrawContents() = 0;
protected:
	std::string	m_Title;
	bool				m_Show;
};

}