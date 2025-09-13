#include <UtilsTestStdafx.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/XMLSerialize.h>


struct GlmVec3 {
	glm::vec3 vec;
	glm::vec3 vecNoSerialize;
};

enum class TestEnum
{
	Value1,
	Value2,
	Value3
};

struct AtomicTypesArrayStruct {
	std::array<int, 3>		   m_IntArray;
	std::array<float, 2>	   m_FloatArray;
	std::array<double, 4>	   m_DoubleArray;
	std::array<bool, 2>		   m_BoolArray;
	std::array<std::string, 3> m_StringArray;
	std::array<TestEnum, 2>	   m_EnumArray;
};

// clang-format off
RTTR_REGISTRATION
{
	
	rttr::registration::class_<GlmVec3>("GlmVec3")
		.property("vecNoSerialize", &GlmVec3::vecNoSerialize)
		(
			Utils::Reflection::RegisterMetamember<Utils::Reflection::SerializationCls::NoSerialize>(true)
		)
		.property("vec", &GlmVec3::vec);
	
	rttr::registration::enumeration<TestEnum>("TestEnum")
		(
			rttr::value("Value1", TestEnum::Value1),
			rttr::value("Value2", TestEnum::Value2),
			rttr::value("Value3", TestEnum::Value3)
		);

	rttr::registration::class_<AtomicTypesArrayStruct>("AtomicTypesArrayStruct")
		.constructor<>()
		.property("intArray", &AtomicTypesArrayStruct::m_IntArray)
		.property("floatArray", &AtomicTypesArrayStruct::m_FloatArray)
		.property("doubleArray", &AtomicTypesArrayStruct::m_DoubleArray)
		.property("boolArray", &AtomicTypesArrayStruct::m_BoolArray)
		.property("stringArray", &AtomicTypesArrayStruct::m_StringArray)
		.property("enumArray", &AtomicTypesArrayStruct::m_EnumArray);
}
// clang-format on

namespace GLEngine::Utils {
class XMLSerializeFixture : public ::testing::Test {
public:
	static std::string ToString(const pugi::xml_document& doc)
	{
		std::stringstream ss;
		doc.save(ss);
		return ss.str();
	}

protected:
	C_XMLSerializer serializer;
};


TEST_F(XMLSerializeFixture, RootName)
{
	GlmVec3	   v{.vec = {1, 2, 3}};
	const auto xmlDoc	= serializer.Serialize(v);
	const auto rootNode = xmlDoc.root().first_child();
	EXPECT_EQ(rootNode.name(), std::string("GlmVec3")) << ToString(xmlDoc);
}

TEST_F(XMLSerializeFixture, NoSerialize)
{
	GlmVec3	   v{.vec = {1, 2, 3}, .vecNoSerialize = {4, 5, 6}};
	const auto xmlDoc	= serializer.Serialize(v);
	const auto rootNode = xmlDoc.root().first_child();
	EXPECT_FALSE(rootNode.attribute("vecNoSerialize")) << ToString(xmlDoc);
}

TEST_F(XMLSerializeFixture, REGISTER_SERIALIZATION)
{
	GlmVec3	   v{.vec = {1, 2, 3}};
	const auto xmlDoc	= serializer.Serialize(v);
	const auto rootNode = xmlDoc.root().first_child();
	EXPECT_EQ(rootNode.name(), std::string("GlmVec3")) << ToString(xmlDoc);
	EXPECT_TRUE(rootNode.attribute("vec")) << ToString(xmlDoc);
	EXPECT_EQ(rootNode.attribute("vec").as_string(), std::string("(1.0,2.0,3.0)")) << ToString(xmlDoc);
}

TEST_F(XMLSerializeFixture, AtomicTypesArrayStructSerialize)
{
	AtomicTypesArrayStruct structObj{};
	structObj.m_IntArray = {1, 2, 3};
	structObj.m_FloatArray = {1.5f, 2.5f};
	structObj.m_DoubleArray = {1.1, 2.2, 3.3, 4.4};
	structObj.m_BoolArray = {true, false};
	structObj.m_StringArray = {"hello", "world", "test"};
	structObj.m_EnumArray = {TestEnum::Value1, TestEnum::Value3};

	const auto xmlDoc = serializer.Serialize(structObj);
	const auto rootNode = xmlDoc.root().first_child();

	EXPECT_EQ(rootNode.name(), std::string("AtomicTypesArrayStruct")) << ToString(xmlDoc);

	// Test int array serialization
	auto intArrayNode = rootNode.child("intArray");
	EXPECT_TRUE(intArrayNode) << ToString(xmlDoc);
	auto intChildren = intArrayNode.children("int");
	int intIndex = 0;
	for (auto child : intChildren)
	{
		EXPECT_EQ(child.attribute("int").as_int(), structObj.m_IntArray[intIndex]) << ToString(xmlDoc);
		intIndex++;
	}
	EXPECT_EQ(intIndex, 3) << ToString(xmlDoc);

	// Test float array serialization
	auto floatArrayNode = rootNode.child("floatArray");
	EXPECT_TRUE(floatArrayNode) << ToString(xmlDoc);
	auto floatChildren = floatArrayNode.children("float");
	int floatIndex = 0;
	for (auto child : floatChildren)
	{
		EXPECT_FLOAT_EQ(child.attribute("float").as_float(), structObj.m_FloatArray[floatIndex]) << ToString(xmlDoc);
		floatIndex++;
	}
	EXPECT_EQ(floatIndex, 2) << ToString(xmlDoc);

	// Test double array serialization
	auto doubleArrayNode = rootNode.child("doubleArray");
	EXPECT_TRUE(doubleArrayNode) << ToString(xmlDoc);
	auto doubleChildren = doubleArrayNode.children("double");
	int doubleIndex = 0;
	for (auto child : doubleChildren)
	{
		EXPECT_DOUBLE_EQ(child.attribute("double").as_double(), structObj.m_DoubleArray[doubleIndex]) << ToString(xmlDoc);
		doubleIndex++;
	}
	EXPECT_EQ(doubleIndex, 4) << ToString(xmlDoc);

	// Test bool array serialization
	auto boolArrayNode = rootNode.child("boolArray");
	EXPECT_TRUE(boolArrayNode) << ToString(xmlDoc);
	auto boolChildren = boolArrayNode.children("bool");
	int boolIndex = 0;
	for (auto child : boolChildren)
	{
		EXPECT_EQ(child.attribute("bool").as_bool(), structObj.m_BoolArray[boolIndex]) << ToString(xmlDoc);
		boolIndex++;
	}
	EXPECT_EQ(boolIndex, 2) << ToString(xmlDoc);

	// Test string array serialization
	auto stringArrayNode = rootNode.child("stringArray");
	EXPECT_TRUE(stringArrayNode) << ToString(xmlDoc);
	auto stringChildren = stringArrayNode.children("std::string");
	int stringIndex = 0;
	for (auto child : stringChildren)
	{
		EXPECT_EQ(child.attribute("std::string").as_string(), structObj.m_StringArray[stringIndex]) << ToString(xmlDoc);
		stringIndex++;
	}
	EXPECT_EQ(stringIndex, 3) << ToString(xmlDoc);

	// Test enum array serialization
	auto enumArrayNode = rootNode.child("enumArray");
	EXPECT_TRUE(enumArrayNode) << ToString(xmlDoc);
	auto enumChildren = enumArrayNode.children("TestEnum");
	int enumIndex = 0;
	std::array<std::string, 2> expectedEnums = {"Value1", "Value3"};
	for (auto child : enumChildren)
	{
		EXPECT_EQ(child.attribute("TestEnum").as_string(), expectedEnums[enumIndex]) << ToString(xmlDoc);
		enumIndex++;
	}
	EXPECT_EQ(enumIndex, 2) << ToString(xmlDoc);
}

} // namespace GLEngine::Utils