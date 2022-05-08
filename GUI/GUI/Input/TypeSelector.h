#pragma once

#include <GUI/GUIApi.h>

#include <GUI/GUIPart.h>

#include <string>

namespace GLEngine::GUI::Input {
class GUI_API_EXPORT C_TypeSelector : public I_GUIPart {
public:
	C_TypeSelector(const std::string_view name, const std::string& baseTypeName);
	void Draw() const override;

	const std::string& GetSelectedTypeName() const { return m_Selected; }

private:
	std::string			m_BaseTypeName;
	std::string			m_Name;
	mutable std::string m_Selected;
};

template<class BaseType>
class GUI_API_EXPORT C_CTTypeSelector : public C_TypeSelector {
public:
	C_CTTypeSelector(const std::string_view name)
		: C_TypeSelector(name, rttr::type::get<BaseType>().get_name())
	{}

	[[nodiscard]] BaseType* GetInstnace() const {

	}
};
} // namespace GLEngine::GUI::Input
