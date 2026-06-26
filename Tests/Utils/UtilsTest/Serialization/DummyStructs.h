#pragma once

#include "../../../../Renderer/Renderer/Colours.h"

enum class TestEnum
{
	Value1,
	Value2,
	Value3
};

struct GlmVec3 {
	glm::vec3 vec;
	glm::vec3 vecNoSerialize;
};

struct AtomicTypesArrayStruct {
	std::array<int, 3>		   m_IntArray;
	std::array<float, 2>	   m_FloatArray;
	std::array<double, 4>	   m_DoubleArray;
	std::array<bool, 2>		   m_BoolArray;
	std::array<std::string, 3> m_StringArray;
	std::array<TestEnum, 2>	   m_EnumArray;
};

struct SetTypesStruct {
	std::set<int>		  m_IntSet;
	std::set<std::string> m_StringSet;
	std::set<TestEnum>	  m_EnumSet;
};

struct VectorOfFloats {
	std::vector<float> m_FloatVec{1.f, 2.f};
};

struct ColourStruct {
	GLEngine::Colours::T_Colour colour;
};

struct RefWrapperStruct {
	int					  m_Int;
	float				  m_Float;
	double				  m_Double;
	bool				  m_Bool;
	char				  m_Char;
	unsigned int		  m_UInt;
	long				  m_Long;
	short				  m_Short;
	std::string			  m_String;
	TestEnum			  m_Enum;
	std::filesystem::path m_Path;
};

struct AtomicTypesStruct {
	int					  m_Int;
	float				  m_Float;
	double				  m_Double;
	bool				  m_Bool;
	char				  m_Char;
	unsigned int		  m_UInt;
	long				  m_Long;
	short				  m_Short;
	std::string			  m_String;
	TestEnum			  m_Enum;
	std::filesystem::path m_Path;
};

struct RefWrapperAtomicTypesArrayStruct {
	std::array<int, 3>		   m_IntArray;
	std::array<float, 2>	   m_FloatArray;
	std::array<double, 4>	   m_DoubleArray;
	std::array<bool, 2>		   m_BoolArray;
	std::array<std::string, 3> m_StringArray;
	std::array<TestEnum, 2>	   m_EnumArray;
};

struct MapStruct {
	std::map<std::string, std::string>					   m_StringToStringMap;
	std::map<std::string, std::filesystem::path>		   m_StringToPathMap;
	std::map<std::filesystem::path, std::string>		   m_PathToStringMap;
	std::map<std::filesystem::path, std::filesystem::path> m_PathToPathMap;
	std::map<int, std::string>							   m_IntToStringMap;
	std::map<std::string, int>							   m_StringToIntMap;
	std::map<float, double>								   m_FloatToDoubleMap;
	std::map<TestEnum, std::string>						   m_EnumToStringMap;
	std::map<std::string, TestEnum>						   m_StringToEnumMap;
	std::map<int, TestEnum>								   m_IntToEnumMap;
};

struct DefaultValueStruct {
	bool WrongBool = false; // REGISTER_DEFAULT_VALUE(true)
};