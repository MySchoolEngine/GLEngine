#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_LambdaPart : public I_GUIPart {
public:
	explicit C_LambdaPart(std::function<void()> lambda);
	virtual void Draw() const override;

private:
	std::function<void()> m_Lambda;
};

} // namespace GLEngine::GUI