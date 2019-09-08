#pragma once

#include <GLRenderer/GUI/GUIPart.h>

namespace GLEngine::GLRenderer::GUI {
class C_Text : public I_GUIPart
{
public:
	template<class ...Args>
	C_Text(const char* fmt, Args&&... args);

	template<class ...Args>
	void SetText(const char* fmt, Args&&... args);

	virtual void Draw() const override;
private:
	std::string m_Text;
};
}

#include <GLRenderer/GUI/Text.inl>