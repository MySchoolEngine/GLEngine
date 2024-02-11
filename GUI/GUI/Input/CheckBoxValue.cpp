#include <GUIStdafx.h>

#include <GUI/Input/CheckBoxValue.h>

namespace GLEngine::GUI::Input {
//=================================================================================
C_CheckBoxValue::C_CheckBoxValue(bool value, std::string&& name)
	: m_Value(value)
	, m_Name(std::move(name))
{
}

//=================================================================================
bool C_CheckBoxValue::Draw() const
{
	return ::ImGui::Checkbox(m_Name.c_str(), &m_Value);
}

//=================================================================================
void C_CheckBoxValue::SetName(std::string&& name)
{
	m_Name = std::move(name);
}

//=================================================================================
bool C_CheckBoxValue::GetValue() const
{
	return m_Value;
}

//=================================================================================
C_CheckBoxValue::operator bool() const
{
	return m_Value;
}

//=================================================================================
bool C_CheckBoxValue::operator||(bool val) const
{
	return m_Value || val;
}

//=================================================================================
bool C_CheckBoxValue::operator&&(bool val) const
{
	return m_Value && val;
}

//=================================================================================
void C_CheckBoxValue::operator|=(bool val)
{
	m_Value |= val;
}

//=================================================================================
void C_CheckBoxValue::operator&=(bool val)
{
	m_Value &= val;
}

//=================================================================================
bool C_CheckBoxValue::operator==(bool val) const
{
	return m_Value == val;
}

//=================================================================================
bool C_CheckBoxValue::operator!=(bool val) const
{
	return !(*this == val);
}

//=================================================================================
C_CheckBoxValue C_CheckBoxValue::operator!()
{
	return C_CheckBoxValue(!m_Value, std::string(m_Name));
}

//=================================================================================
bool* C_CheckBoxValue::operator&() const
{
	return &m_Value;
}

//=================================================================================
bool C_CheckBoxValue::operator=(bool val)
{
	return m_Value = val;
}

} // namespace GLEngine::GUI::Input