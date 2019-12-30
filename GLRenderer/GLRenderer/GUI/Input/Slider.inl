#pragma once

namespace GLEngine::GLRenderer::GUI::Input {

//=================================================================================
template<class T>
C_Slider<T>::C_Slider(T value, T min, T max, std::string&& name /*= ""*/)
	: m_Value(value)
	, m_Prev(value)
	, m_Min(min)
	, m_Max(max)
	, m_Name(std::move(name))
{

}

//=================================================================================
template<class T>
C_Slider<T>::operator T() const
{
	return GetValue();
}

//=================================================================================
template<typename T>
T* C_Slider<T>::operator&() const
{
	return &m_Value;
}

//=================================================================================
template<typename T>
float C_Slider<T>::operator=(T val)
{
	return m_Value = m_Prev = val;
}

//=================================================================================
template<class T>
T C_Slider<T>::GetValue() const
{
	return m_Value;
}

//=================================================================================
template<class T>
void C_Slider<T>::SetName(std::string&& name)
{
	m_Name = std::move(name);
}

//=================================================================================
template<class T>
T C_Slider<T>::GetPreviousValue() const
{
	return m_Prev;
}

//=================================================================================
template<class T>
bool C_Slider<T>::Changed() const
{
	return m_Prev != m_Value;
}

//=================================================================================
template<typename T>
void C_Slider<T>::operator+=(T val)
{
	m_Value += val;
}

//=================================================================================
template<typename T>
void C_Slider<T>::operator-=(T val)
{
	m_Value -= val;
}

//=================================================================================
template<typename T>
C_Slider<T> C_Slider<T>::operator+(T val) const
{
	return C_Slider(m_Value + val, m_Min, m_Max);
}

//=================================================================================
template<typename T>
C_Slider<T> C_Slider<T>::operator-(T val) const
{
	return C_Slider(m_Value - val, m_Min, m_Max);
}

//=================================================================================
template<typename T>
bool C_Slider<T>::operator!=(T val) const
{
	return m_Value != val;
}

//=================================================================================
template<typename T>
bool C_Slider<T>::operator==(T val) const
{
	return m_Value == val;
}

}