#pragma once

#include <GLRenderer/GUI/GUIPart.h>

namespace GLEngine {
namespace GLRenderer {
namespace GUI {

// @todo unit testing
template<typename T>
class C_Slider : public I_GUIPart {
public:
	C_Slider(T value, T min, T max, std::string&& name = "");
	virtual void Draw() const override;

	void SetName(std::string&& name);
	T GetValue() const;

	bool Changed() const;
	T GetPreviousValue() const;

	operator T() const;
	T* operator&() const;
	float operator=(T val);
	bool operator==(T val) const;
	bool operator!=(T val) const;
	void operator-=(T val);
	void operator+=(T val);
	C_Slider<T> operator-(T val) const;
	C_Slider<T> operator+(T val) const;
private:
	mutable T m_Value;
	mutable T m_Prev;
	T m_Min;
	T m_Max;
	std::string m_Name;
};

}}}

#include <GLRenderer/GUI/Slider.inl>