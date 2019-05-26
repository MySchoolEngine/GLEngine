#pragma once

#include <Renderer/IDebugGUIComponent.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {

class C_GLDebugGUIComponent : public Renderer::I_DebugGUIComponent {
public:
	C_GLDebugGUIComponent(const std::string& name);
	virtual ~C_GLDebugGUIComponent() = default;

	//=====================================
	virtual void Toggle() override;
	virtual void PostUpdate() override;

	virtual void DrawContents() = 0;
protected:
	std::string m_Title;
	bool		m_Show;
};

}
}
}