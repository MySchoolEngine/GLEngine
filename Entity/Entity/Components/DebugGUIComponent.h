#pragma once

#include <Entity/Components/IDebugGUIComponent.h>

namespace GLEngine::Entity {

class C_DebugGUIComponent : public I_DebugGUIComponent {
public:
	C_DebugGUIComponent(std::shared_ptr<I_Entity> owner);
	virtual ~C_DebugGUIComponent() = default;

	//=====================================
	virtual void Toggle() override;
	virtual void PostUpdate() override;

	virtual void DrawContents() = 0;
protected:
	std::string	m_Title;
	bool				m_Show;
};

}