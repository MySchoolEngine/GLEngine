#pragma once

#include <GUI/GUIApi.h>

#include <GUI/GUIPart.h>

#include <string>

namespace GLEngine::GUI::Input {
class GUI_API_EXPORT C_TypeSelector : public I_GUIPart {
public:
	C_TypeSelector(const std::string_view name, const std::string& baseTypeName);
	bool Draw() const override;

	const std::string& GetSelectedTypeName() const { return m_Selected; }

private:
	std::string			m_BaseTypeName;
	std::string			m_Name;
	mutable std::string m_Selected;
};
} // namespace GLEngine::GUI::Input
