#include <UtilsTestStdafx.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/XMLDeserialize.h>

#include <pugixml.hpp>

#include "../../../../Renderer/Renderer/Colours.h"
#include <CommonTestUtils/XMLDeserializeFixture.h>


struct GlmVec3 {
	glm::vec3 vec;
	glm::vec3 vecNoSerialize;
};

struct ColourStruct {
	GLEngine::Colours::T_Colour colour;
};

enum class TestEnum
{
	Value1,
	Value2,
	Value3
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

struct AtomicTypesArrayStruct {
	std::array<int, 3>		   m_IntArray;
	std::array<float, 2>	   m_FloatArray;
	std::array<double, 4>	   m_DoubleArray;
	std::array<bool, 2>		   m_BoolArray;
	std::array<std::string, 3> m_StringArray;
	std::array<TestEnum, 2>	   m_EnumArray;
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

struct SetTypesStruct {
	std::set<int>		  m_IntSet;
	std::set<std::string> m_StringSet;
	std::set<TestEnum>	  m_EnumSet;
};
// clang-format off
RTTR_REGISTRATION
{
	rttr::registration::class_<GlmVec3>("GlmVec3")
		.constructor<>()
		.property("vecNoSerialize", &GlmVec3::vecNoSerialize)
		(
			Utils::Reflection::RegisterMetamember<Utils::Reflection::SerializationCls::NoSerialize>(true)
		)
		.property("vec", &GlmVec3::vec);
	
	rttr::registration::class_<ColourStruct>("ColourStruct")
		.constructor<>()
		.property("colour", &ColourStruct::colour);
	
	rttr::registration::enumeration<TestEnum>("TestEnum")
		(
			rttr::value("Value1", TestEnum::Value1),
			rttr::value("Value2", TestEnum::Value2),
			rttr::value("Value3", TestEnum::Value3)
		);
	
	rttr::registration::class_<RefWrapperStruct>("RefWrapperStruct")
		.constructor<>()
		.property("int", &RefWrapperStruct::m_Int)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("float", &RefWrapperStruct::m_Float)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("double", &RefWrapperStruct::m_Double)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("bool", &RefWrapperStruct::m_Bool)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("char", &RefWrapperStruct::m_Char)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("uint", &RefWrapperStruct::m_UInt)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("long", &RefWrapperStruct::m_Long)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("short", &RefWrapperStruct::m_Short)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("string", &RefWrapperStruct::m_String)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("enum", &RefWrapperStruct::m_Enum)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("path", &RefWrapperStruct::m_Path)
			(
				rttr::policy::prop::as_reference_wrapper
			);
	
	rttr::registration::class_<AtomicTypesStruct>("AtomicTypesStruct")
		.constructor<>()
		.property("int", &AtomicTypesStruct::m_Int)
		.property("float", &AtomicTypesStruct::m_Float)
		.property("double", &AtomicTypesStruct::m_Double)
		.property("bool", &AtomicTypesStruct::m_Bool)
		.property("char", &AtomicTypesStruct::m_Char)
		.property("uint", &AtomicTypesStruct::m_UInt)
		.property("long", &AtomicTypesStruct::m_Long)
		.property("short", &AtomicTypesStruct::m_Short)
		.property("string", &AtomicTypesStruct::m_String)
		.property("enum", &AtomicTypesStruct::m_Enum)
		.property("path", &AtomicTypesStruct::m_Path);

	rttr::registration::class_<AtomicTypesArrayStruct>("AtomicTypesArrayStruct")
		.constructor<>()
		.property("intArray", &AtomicTypesArrayStruct::m_IntArray)
		.property("floatArray", &AtomicTypesArrayStruct::m_FloatArray)
		.property("doubleArray", &AtomicTypesArrayStruct::m_DoubleArray)
		.property("boolArray", &AtomicTypesArrayStruct::m_BoolArray)
		.property("stringArray", &AtomicTypesArrayStruct::m_StringArray)
		.property("enumArray", &AtomicTypesArrayStruct::m_EnumArray);

	rttr::registration::class_<RefWrapperAtomicTypesArrayStruct>("RefWrapperAtomicTypesArrayStruct")
		.constructor<>()
		.property("intArray", &RefWrapperAtomicTypesArrayStruct::m_IntArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("floatArray", &RefWrapperAtomicTypesArrayStruct::m_FloatArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("doubleArray", &RefWrapperAtomicTypesArrayStruct::m_DoubleArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("boolArray", &RefWrapperAtomicTypesArrayStruct::m_BoolArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("stringArray", &RefWrapperAtomicTypesArrayStruct::m_StringArray)
			(
				rttr::policy::prop::as_reference_wrapper
			)
		.property("enumArray", &RefWrapperAtomicTypesArrayStruct::m_EnumArray)
			(
				rttr::policy::prop::as_reference_wrapper
			);

	rttr::registration::class_<MapStruct>("MapStruct")
		.constructor<>()
		.property("stringToStringMap", &MapStruct::m_StringToStringMap)
		.property("stringToPathMap", &MapStruct::m_StringToPathMap)
		.property("pathToStringMap", &MapStruct::m_PathToStringMap)
		.property("pathToPathMap", &MapStruct::m_PathToPathMap)
		.property("intToStringMap", &MapStruct::m_IntToStringMap)
		.property("stringToIntMap", &MapStruct::m_StringToIntMap)
		.property("floatToDoubleMap", &MapStruct::m_FloatToDoubleMap)
		.property("enumToStringMap", &MapStruct::m_EnumToStringMap)
		.property("stringToEnumMap", &MapStruct::m_StringToEnumMap)
		.property("intToEnumMap", &MapStruct::m_IntToEnumMap);

	rttr::registration::class_<SetTypesStruct>("SetTypesStruct")
		.constructor<>()
		.property("intSet", &SetTypesStruct::m_IntSet)
		.property("stringSet", &SetTypesStruct::m_StringSet)
		.property("enumSet", &SetTypesStruct::m_EnumSet);
}
// clang-format on

namespace GLEngine::Utils {


TEST_F(XMLDeserializeFixture, Vec3Deserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<GlmVec3 vec="(1,2,3)" />)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<GlmVec3>>(doc);
	EXPECT_TRUE(result.has_value());
}

TEST_F(XMLDeserializeFixture, ColourDeserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<ColourStruct colour="(1,2,3)" />)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<ColourStruct>>(doc);
	EXPECT_TRUE(result.has_value());
}

TEST_F(XMLDeserializeFixture, RefWrapperStructDeserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<RefWrapperStruct 
	int="42" 
	float="3.14" 
	double="2.718281828" 
	bool="true" 
	char="A" 
	uint="100" 
	long="123456789" 
	short="255" 
	string="test string" 
	enum="Value2" 
	path="test/path/file.txt" />)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<RefWrapperStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	if (result.has_value())
	{
		const auto& obj = result.value();

		// Test arithmetic types
		EXPECT_EQ(obj->m_Int, 42);
		EXPECT_FLOAT_EQ(obj->m_Float, 3.14f);
		EXPECT_DOUBLE_EQ(obj->m_Double, 2.718281828);
		EXPECT_TRUE(obj->m_Bool);
		EXPECT_EQ(obj->m_Char, 'A');
		EXPECT_EQ(obj->m_UInt, 100u);
		//EXPECT_EQ(obj->m_Long, 123456789L);
		EXPECT_EQ(obj->m_Short, 255);

		// Test string type
		EXPECT_EQ(obj->m_String, "test string");

		// Test enum type
		EXPECT_EQ(obj->m_Enum, TestEnum::Value2);

		// Test filesystem::path type
		EXPECT_EQ(obj->m_Path, std::filesystem::path("test/path/file.txt"));
	}
}

// disabled due to errors on Linux, Will be fixed once I have access to Linux machine
TEST_F(XMLDeserializeFixture, DISABLED_AtomicTypesStructDeserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<AtomicTypesStruct
	int="42"
	float="3.14"
	double="2.718281828"
	bool="true"
	char="A"
	uint="100"
	long="123456789"
	short="255"
	string="test string"
	enum="Value2"
	path="test/path/file.txt" />)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<AtomicTypesStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	if (result.has_value())
	{
		const auto& obj = result.value();

		// Test arithmetic types
		EXPECT_EQ(obj->m_Int, 42);
		EXPECT_FLOAT_EQ(obj->m_Float, 3.14f);
		EXPECT_DOUBLE_EQ(obj->m_Double, 2.718281828);
		EXPECT_TRUE(obj->m_Bool);
		EXPECT_EQ(obj->m_Char, 'A');
		EXPECT_EQ(obj->m_UInt, 100u);
		//EXPECT_EQ(obj->m_Long, 123456789L);
		EXPECT_EQ(obj->m_Short, 255);

		// Test string type
		EXPECT_EQ(obj->m_String, "test string");

		// Test enum type
		EXPECT_EQ(obj->m_Enum, TestEnum::Value2);

		// Test filesystem::path type
		EXPECT_EQ(obj->m_Path, std::filesystem::path("test/path/file.txt"));
	}
}

// disabled due to errors on Linux, Will be fixed once I have access to Linux machine
TEST_F(XMLDeserializeFixture, DISABLED_AtomicTypesStructDeserializeChildElements)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<AtomicTypesStruct>
	<int int="42" />
	<float float="3.14" />
	<double double="2.718281828" />
	<bool bool="true" />
	<char char="A" />
	<uint uint="100" />
	<long long="123456789" />
	<short short="255" />
	<string string="test string" />
	<enum enum="Value2" />
	<path path="test/path/file.txt" />
</AtomicTypesStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<AtomicTypesStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	if (result.has_value())
	{
		const auto& obj = result.value();

		// Test arithmetic types
		EXPECT_EQ(obj->m_Int, 42);
		EXPECT_FLOAT_EQ(obj->m_Float, 3.14f);
		EXPECT_DOUBLE_EQ(obj->m_Double, 2.718281828);
		EXPECT_TRUE(obj->m_Bool);
		EXPECT_EQ(obj->m_Char, 'A');
		EXPECT_EQ(obj->m_UInt, 100u);
		EXPECT_EQ(obj->m_Long, 123456789L);
		EXPECT_EQ(obj->m_Short, 255);

		// Test string type
		EXPECT_EQ(obj->m_String, "test string");

		// Test enum type
		EXPECT_EQ(obj->m_Enum, TestEnum::Value2);

		// Test filesystem::path type
		EXPECT_EQ(obj->m_Path, std::filesystem::path("test/path/file.txt"));
	}
}

// disabled due to errors on Linux, Will be fixed once I have access to Linux machine
TEST_F(XMLDeserializeFixture, DISABLED_RefWrapperStructDeserializeChildElements)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<RefWrapperStruct>
	<int int="42" />
	<float float="3.14" />
	<double double="2.718281828" />
	<bool bool="true" />
	<char char="A" />
	<uint uint="100" />
	<long long="123456789" />
	<short short="255" />
	<string string="test string" />
	<enum enum="Value2" />
	<path path="test/path/file.txt" />
</RefWrapperStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<RefWrapperStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	if (result.has_value())
	{
		const auto& obj = result.value();

		// Test arithmetic types
		EXPECT_EQ(obj->m_Int, 42);
		EXPECT_FLOAT_EQ(obj->m_Float, 3.14f);
		EXPECT_DOUBLE_EQ(obj->m_Double, 2.718281828);
		EXPECT_TRUE(obj->m_Bool);
		EXPECT_EQ(obj->m_Char, 'A');
		EXPECT_EQ(obj->m_UInt, 100u);
		//EXPECT_EQ(obj->m_Long, 123456789L);
		EXPECT_EQ(obj->m_Short, 255);

		// Test string type
		EXPECT_EQ(obj->m_String, "test string");

		// Test enum type
		EXPECT_EQ(obj->m_Enum, TestEnum::Value2);

		// Test filesystem::path type
		EXPECT_EQ(obj->m_Path, std::filesystem::path("test/path/file.txt"));
	}
}

TEST_F(XMLDeserializeFixture, AtomicTypesArrayStructDeserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<AtomicTypesArrayStruct>
	<intArray>
		<int int="1" />
		<int int="2" />
		<int int="3" />
	</intArray>
	<floatArray>
		<float float="1.5" />
		<float float="2.5" />
	</floatArray>
	<doubleArray>
		<double double="1.1" />
		<double double="2.2" />
		<double double="3.3" />
		<double double="4.4" />
	</doubleArray>
	<boolArray>
		<bool bool="true" />
		<bool bool="false" />
	</boolArray>
	<stringArray>
		<std::string std::string="hello" />
		<std::string std::string="world" />
		<std::string std::string="test" />
	</stringArray>
	<enumArray>
		<TestEnum TestEnum="Value1" />
		<TestEnum TestEnum="Value3" />
	</enumArray>
</AtomicTypesArrayStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<AtomicTypesArrayStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	if (result.has_value())
	{
		const auto& obj = result.value();

		// Test int array
		EXPECT_EQ(obj->m_IntArray[0], 1);
		EXPECT_EQ(obj->m_IntArray[1], 2);
		EXPECT_EQ(obj->m_IntArray[2], 3);

		// Test float array
		EXPECT_FLOAT_EQ(obj->m_FloatArray[0], 1.5f);
		EXPECT_FLOAT_EQ(obj->m_FloatArray[1], 2.5f);

		// Test double array
		EXPECT_DOUBLE_EQ(obj->m_DoubleArray[0], 1.1);
		EXPECT_DOUBLE_EQ(obj->m_DoubleArray[1], 2.2);
		EXPECT_DOUBLE_EQ(obj->m_DoubleArray[2], 3.3);
		EXPECT_DOUBLE_EQ(obj->m_DoubleArray[3], 4.4);

		// Test bool array
		EXPECT_TRUE(obj->m_BoolArray[0]);
		EXPECT_FALSE(obj->m_BoolArray[1]);

		// Test string array
		EXPECT_EQ(obj->m_StringArray[0], "hello");
		EXPECT_EQ(obj->m_StringArray[1], "world");
		EXPECT_EQ(obj->m_StringArray[2], "test");

		// Test enum array
		EXPECT_EQ(obj->m_EnumArray[0], TestEnum::Value1);
		EXPECT_EQ(obj->m_EnumArray[1], TestEnum::Value3);
	}
}

TEST_F(XMLDeserializeFixture, RefWrapperAtomicTypesArrayStructDeserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<RefWrapperAtomicTypesArrayStruct>
	<intArray>
		<int int="1" />
		<int int="2" />
		<int int="3" />
	</intArray>
	<floatArray>
		<float float="1.5" />
		<float float="2.5" />
	</floatArray>
	<doubleArray>
		<double double="1.1" />
		<double double="2.2" />
		<double double="3.3" />
		<double double="4.4" />
	</doubleArray>
	<boolArray>
		<bool bool="true" />
		<bool bool="false" />
	</boolArray>
	<stringArray>
		<std::string std::string="hello" />
		<std::string std::string="world" />
		<std::string std::string="test" />
	</stringArray>
	<enumArray>
		<TestEnum TestEnum="Value1" />
		<TestEnum TestEnum="Value3" />
	</enumArray>
</RefWrapperAtomicTypesArrayStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<RefWrapperAtomicTypesArrayStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	if (result.has_value())
	{
		const auto& obj = result.value();

		// Test int array
		EXPECT_EQ(obj->m_IntArray[0], 1);
		EXPECT_EQ(obj->m_IntArray[1], 2);
		EXPECT_EQ(obj->m_IntArray[2], 3);

		// Test float array
		EXPECT_FLOAT_EQ(obj->m_FloatArray[0], 1.5f);
		EXPECT_FLOAT_EQ(obj->m_FloatArray[1], 2.5f);

		// Test double array
		EXPECT_DOUBLE_EQ(obj->m_DoubleArray[0], 1.1);
		EXPECT_DOUBLE_EQ(obj->m_DoubleArray[1], 2.2);
		EXPECT_DOUBLE_EQ(obj->m_DoubleArray[2], 3.3);
		EXPECT_DOUBLE_EQ(obj->m_DoubleArray[3], 4.4);

		// Test bool array
		EXPECT_TRUE(obj->m_BoolArray[0]);
		EXPECT_FALSE(obj->m_BoolArray[1]);

		// Test string array
		EXPECT_EQ(obj->m_StringArray[0], "hello");
		EXPECT_EQ(obj->m_StringArray[1], "world");
		EXPECT_EQ(obj->m_StringArray[2], "test");

		// Test enum array
		EXPECT_EQ(obj->m_EnumArray[0], TestEnum::Value1);
		EXPECT_EQ(obj->m_EnumArray[1], TestEnum::Value3);
	}
}

TEST_F(XMLDeserializeFixture, MapStructDeserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<MapStruct>
	<stringToStringMap>
		<item>
			<key value="key1" />
			<value value="value1" />
		</item>
		<item>
			<key value="key2" />
			<value value="value2" />
		</item>
		<item>
			<key value="key3" />
			<value value="value3" />
		</item>
	</stringToStringMap>
	<stringToPathMap>
		<item>
			<key value="config" />
			<value value="path/to/config.txt" />
		</item>
		<item>
			<key value="data" />
			<value value="path/to/data.bin" />
		</item>
	</stringToPathMap>
	<pathToStringMap>
		<item>
			<key value="home/user/file1.txt" />
			<value value="description1" />
		</item>
		<item>
			<key value="home/user/file2.txt" />
			<value value="description2" />
		</item>
	</pathToStringMap>
	<pathToPathMap>
		<item>
			<key value="src/main.cpp" />
			<value value="build/main.obj" />
		</item>
		<item>
			<key value="src/utils.cpp" />
			<value value="build/utils.obj" />
		</item>
	</pathToPathMap>
	<intToStringMap>
		<item>
			<key value="1" />
			<value value="one" />
		</item>
		<item>
			<key value="42" />
			<value value="answer" />
		</item>
	</intToStringMap>
	<stringToIntMap>
		<item>
			<key value="width" />
			<value value="1920" />
		</item>
		<item>
			<key value="height" />
			<value value="1080" />
		</item>
	</stringToIntMap>
	<floatToDoubleMap>
		<item>
			<key value="3.14" />
			<value value="3.141592653589793" />
		</item>
		<item>
			<key value="2.71" />
			<value value="2.718281828459045" />
		</item>
	</floatToDoubleMap>
	<enumToStringMap>
		<item>
			<key value="Value1" />
			<value value="First Value" />
		</item>
		<item>
			<key value="Value2" />
			<value value="Second Value" />
		</item>
	</enumToStringMap>
	<stringToEnumMap>
		<item>
			<key value="option1" />
			<value value="Value1" />
		</item>
		<item>
			<key value="option3" />
			<value value="Value3" />
		</item>
	</stringToEnumMap>
	<intToEnumMap>
		<item>
			<key value="0" />
			<value value="Value1" />
		</item>
		<item>
			<key value="2" />
			<value value="Value3" />
		</item>
	</intToEnumMap>
</MapStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<MapStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	if (result.has_value())
	{
		const auto& obj = result.value();

		// Test string to string map
		EXPECT_EQ(obj->m_StringToStringMap.size(), 3);
		EXPECT_EQ(obj->m_StringToStringMap.at("key1"), "value1");
		EXPECT_EQ(obj->m_StringToStringMap.at("key2"), "value2");
		EXPECT_EQ(obj->m_StringToStringMap.at("key3"), "value3");

		// Test string to path map
		EXPECT_EQ(obj->m_StringToPathMap.size(), 2);
		EXPECT_EQ(obj->m_StringToPathMap.at("config"), std::filesystem::path("path/to/config.txt"));
		EXPECT_EQ(obj->m_StringToPathMap.at("data"), std::filesystem::path("path/to/data.bin"));

		// Test path to string map
		EXPECT_EQ(obj->m_PathToStringMap.size(), 2);
		EXPECT_EQ(obj->m_PathToStringMap.at(std::filesystem::path("home/user/file1.txt")), "description1");
		EXPECT_EQ(obj->m_PathToStringMap.at(std::filesystem::path("home/user/file2.txt")), "description2");

		// Test path to path map
		EXPECT_EQ(obj->m_PathToPathMap.size(), 2);
		EXPECT_EQ(obj->m_PathToPathMap.at(std::filesystem::path("src/main.cpp")), std::filesystem::path("build/main.obj"));
		EXPECT_EQ(obj->m_PathToPathMap.at(std::filesystem::path("src/utils.cpp")), std::filesystem::path("build/utils.obj"));

		// Test int to string map
		EXPECT_EQ(obj->m_IntToStringMap.size(), 2);
		EXPECT_EQ(obj->m_IntToStringMap.at(1), "one");
		EXPECT_EQ(obj->m_IntToStringMap.at(42), "answer");

		// Test string to int map
		EXPECT_EQ(obj->m_StringToIntMap.size(), 2);
		EXPECT_EQ(obj->m_StringToIntMap.at("width"), 1920);
		EXPECT_EQ(obj->m_StringToIntMap.at("height"), 1080);

		// Test float to double map
		EXPECT_EQ(obj->m_FloatToDoubleMap.size(), 2);
		EXPECT_DOUBLE_EQ(obj->m_FloatToDoubleMap.at(3.14f), 3.141592653589793);
		EXPECT_DOUBLE_EQ(obj->m_FloatToDoubleMap.at(2.71f), 2.718281828459045);

		// Test enum to string map
		EXPECT_EQ(obj->m_EnumToStringMap.size(), 2);
		EXPECT_EQ(obj->m_EnumToStringMap.at(TestEnum::Value1), "First Value");
		EXPECT_EQ(obj->m_EnumToStringMap.at(TestEnum::Value2), "Second Value");

		// Test string to enum map
		EXPECT_EQ(obj->m_StringToEnumMap.size(), 2);
		EXPECT_EQ(obj->m_StringToEnumMap.at("option1"), TestEnum::Value1);
		EXPECT_EQ(obj->m_StringToEnumMap.at("option3"), TestEnum::Value3);

		// Test int to enum map
		EXPECT_EQ(obj->m_IntToEnumMap.size(), 2);
		EXPECT_EQ(obj->m_IntToEnumMap.at(0), TestEnum::Value1);
		EXPECT_EQ(obj->m_IntToEnumMap.at(2), TestEnum::Value3);
	}
}

TEST_F(XMLDeserializeFixture, SetTypesStructDeserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<SetTypesStruct>
	<intSet>
		<item>
			<key value="1" />
		</item>
		<item>
			<key value="2" />
		</item>
		<item>
			<key value="3" />
		</item>
		<item>
			<key value="5" />
		</item>
	</intSet>
	<stringSet>
		<item>
			<key value="Models/lada/lada.tri" />
		</item>
		<item>
			<key value="hello" />
		</item>
		<item>
			<key value="world" />
		</item>
	</stringSet>
	<enumSet>
		<item>
			<key value="Value1" />
		</item>
		<item>
			<key value="Value3" />
		</item>
	</enumSet>
</SetTypesStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<SetTypesStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	if (result.has_value())
	{
		const auto& obj = result.value();

		// Test int set
		EXPECT_EQ(obj->m_IntSet.size(), 4);
		EXPECT_TRUE(obj->m_IntSet.contains(1));
		EXPECT_TRUE(obj->m_IntSet.contains(2));
		EXPECT_TRUE(obj->m_IntSet.contains(3));
		EXPECT_TRUE(obj->m_IntSet.contains(5));

		// Test string set
		EXPECT_EQ(obj->m_StringSet.size(), 3);
		EXPECT_TRUE(obj->m_StringSet.contains("Models/lada/lada.tri"));
		EXPECT_TRUE(obj->m_StringSet.contains("hello"));
		EXPECT_TRUE(obj->m_StringSet.contains("world"));

		// Test enum set
		EXPECT_EQ(obj->m_EnumSet.size(), 2);
		EXPECT_TRUE(obj->m_EnumSet.contains(TestEnum::Value1));
		EXPECT_TRUE(obj->m_EnumSet.contains(TestEnum::Value3));
	}
}

} // namespace GLEngine::Utils