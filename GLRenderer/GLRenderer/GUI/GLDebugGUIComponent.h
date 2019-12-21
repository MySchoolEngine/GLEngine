#pragma once

#include <Renderer/IDebugGUIComponent.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {

class C_GLDebugGUIComponent : public Renderer::I_DebugGUIComponent {
public:
	C_GLDebugGUIComponent(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_GLDebugGUIComponent() = default;

	//=====================================
	virtual void Toggle() override;
	virtual void PostUpdate() override;

	void SetTitle(std::string& title);

	virtual void DrawContents() = 0;
protected:
	std::string m_Title;
	bool		m_Show;
};

}
}
}