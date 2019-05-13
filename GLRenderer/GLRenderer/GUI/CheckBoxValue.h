#pragma once


namespace GLEngine {
namespace GLRenderer {
namespace GUI {

// This should look like bool value for user and has size of bool value
class C_CheckBoxValue {
public:
	C_CheckBoxValue(bool value, std::string&& name = "");
	void Draw() const;

	void SetName(std::string&& name);

	explicit operator bool() const;
	bool operator=(bool val);
	bool* operator&() const;
private:
	std::string m_Name;
	// it is mutable anyway as imgui is free to change its value
	mutable bool		m_Value;
};

}}}