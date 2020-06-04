#pragma once

#include <GLRenderer/GUI/GUIPart.h>

// callback and setter options

namespace GLEngine::GLRenderer::GUI {
class C_FormatedText : public I_GUIPart {
public:
	C_FormatedText(const std::string& formatText);
	~C_FormatedText() = default;

	C_FormatedText(const C_FormatedText&& other);
	C_FormatedText(const C_FormatedText& other);

	virtual void Draw() const override;

	[[nodiscard]] const std::string_view GetCurrentText() const;

	template<class ...Args>
	void UpdateText(Args&&... args);
protected:
	std::string m_FormatText;
	std::string m_PrintString;
};
}

#include <GLRenderer/GUI/Text.inl>