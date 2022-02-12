#pragma once

#include <GUI/GUIPart.h>
#include <Renderer/Colours.h>

namespace GLEngine::GUI::Input {

template <class T> class C_ColorBase : public I_GUIPart {
public:
	C_ColorBase(std::string&& name, T def)
		: m_name(std::move(name))
		, m_Color(def)
	{
	}

	[[nodiscard]] const T& GetValue() const { return m_Color; }
	void				   SetValue(T&& value) { m_Color = value; }

	void operator=(const T& val) { m_Color = val; }
	void operator=(T&& val) { m_Color = val; }

protected:
	std::string m_name;
	mutable T	m_Color;
};

class C_ColorRBG : public C_ColorBase<Colours::T_Colour> {
public:
	C_ColorRBG(std::string&& name, Colours::T_Colour def)
		: C_ColorBase(std::move(name), (def))
	{
	}
	GUI_API_EXPORT virtual void Draw() const override;


	using C_ColorBase<Colours::T_Colour>::operator=;
};


class C_ColorRBGA : public C_ColorBase<glm::vec4> {
public:
	C_ColorRBGA(std::string&& name, glm::vec4 def)
		: C_ColorBase(std::move(name), (def))
	{
	}
	GUI_API_EXPORT virtual void Draw() const override;


	using C_ColorBase<glm::vec4>::operator=;
};
} // namespace GLEngine::GUI::Input
