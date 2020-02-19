#pragma once

#include <GLRenderer/GUI/GUIPart.h>

namespace GLEngine::GLRenderer::GUI::Input {
class C_Button : public I_GUIPart {
public:
	C_Button(std::string&& name, const std::function<void()>& callback);

	virtual void Draw() const override;
private:
	std::string						m_name;
	std::function<void()> m_Callback;
};
}