#pragma once

#include <GLRenderer/GUI/GUIPart.h>

#include <glm/glm.hpp>

namespace GLEngine::GLRenderer::GUI::Input {

template<class T>
class C_Vector : public I_GUIPart {
public:
	C_Vector(std::string&& name, T default)
		: m_name(std::move(name))
		, m_Value(default) {}

	const T& GetValue() const
	{
		return m_Value;
	}
	void SetValue(T&& value)
	{
		m_Value = value;
	}
protected:
	std::string				m_name;
	mutable T					m_Value;
};

//=================================================================================
class C_Vec3 final : public C_Vector<glm::vec3>
{
public:
	C_Vec3(std::string&& name, glm::vec3 default);

	void Draw() const override;

	C_Vec3& operator=(const glm::vec3& value);
};

}