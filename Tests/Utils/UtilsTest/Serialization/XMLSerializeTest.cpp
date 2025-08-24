#include <UtilsTestStdafx.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/XMLSerialize.h>

#include <rttr/registration>


struct GlmVec3 {
	glm::vec3 vec;
	glm::vec3 vecNoSerialize;
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

} // namespace GLEngine::Utils