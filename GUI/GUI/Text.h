#pragma once

#include <GUI/GUIPart.h>

// callback and setter options

namespace GLEngine::GUI {
class C_FormatedText : public I_GUIPart {
public:
	GUI_API_EXPORT C_FormatedText(const std::string& formatText);
	~C_FormatedText() = default;

	GUI_API_EXPORT C_FormatedText(const C_FormatedText&& other);
	GUI_API_EXPORT C_FormatedText(const C_FormatedText& other);

	virtual bool Draw() const override;

	GUI_API_EXPORT [[nodiscard]] const std::string_view GetCurrentText() const;

	template <class... Args> void UpdateText(Args&&... args);

protected:
	std::string m_FormatText;
	std::string m_PrintString;
};
} // namespace GLEngine::GUI

#include <GUI/Text.inl>