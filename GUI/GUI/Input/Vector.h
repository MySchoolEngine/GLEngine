#pragma once

#include <GUI/GUIPart.h>

namespace GLEngine::GUI::Input {

template <class T> class C_Vector : public I_GUIPart {
public:
	C_Vector(std::string&& name, T val)
		: m_Name(std::move(name))
		, m_Value(val)
	{
	}

	[[nodiscard]] const T& GetValue() const { return m_Value; }
	void				   SetValue(T&& value) { m_Value = std::move(value); }

protected:
	std::string m_Name;
	mutable T	m_Value;
};

//=================================================================================
class GUI_API_EXPORT C_Vec3 final : public C_Vector<glm::vec3> {
public:
	C_Vec3(std::string&& name, glm::vec3 val);

	bool Draw() const override;

	C_Vec3& operator=(const glm::vec3& value);
};

} // namespace GLEngine::GUI::Input