#pragma once

#include <GLRenderer/GUI/GUIPart.h>

namespace GLEngine::GLRenderer::GUI {
class C_LambdaPart : public I_GUIPart {
public:
	explicit C_LambdaPart(std::function<void()> lambda);
	virtual void Draw() const override;
private:
	std::function<void()> m_Lambda;
};

}