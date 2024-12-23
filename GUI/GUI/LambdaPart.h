#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GUI {
class GUI_API_EXPORT C_LambdaPart final : public I_GUIPart {
public:
	explicit C_LambdaPart(const std::function<bool()>& lambda);
	bool Draw() const override;

private:
	std::function<bool()> m_Lambda;
};

} // namespace GLEngine::GUI