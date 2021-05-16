#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GUI::Input {

template <class Key> class C_Select : public I_GUIPart {
public:
	C_Select(const std::string_view name, const Key& defaultKey, const std::string_view label);
	virtual void Draw() const override;

	void	   AddValue(const Key& key, const std::string_view label);
	const Key& GetSelectedValue() const;

	[[nodiscard]] bool Changed() const;

private:
	using T_Container = std::map<Key, std::string>;
	T_Container									 m_Values;
	mutable typename T_Container::const_iterator m_Selected;
	mutable typename T_Container::const_iterator m_Previous;
	std::string									 m_Name;
};
} // namespace GLEngine::GUI::Input

#include <GUI/Input/Select.inl>
