#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_LambdaPart : public I_GUIPart {
public:
	explicit C_LambdaPart(std::function<bool()> lambda);
	virtual bool Draw() const override;

private:
	std::function<bool()> m_Lambda;
};

} // namespace GLEngine::GUI