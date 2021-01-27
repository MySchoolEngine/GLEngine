#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GUI::Input {
class GUI_API_EXPORT C_Button : public I_GUIPart {
public:
	C_Button(std::string&& name, const std::function<void()>& callback);

	virtual void Draw() const override;

private:
	std::string			  m_name;
	std::function<void()> m_Callback;
};
} // namespace GLEngine::GUI::Input