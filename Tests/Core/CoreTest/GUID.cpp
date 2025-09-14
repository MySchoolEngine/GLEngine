#include <CoreTestStdafx.h>

#include <Core/GUID.h>

#include <CommonTestUtils/XMLDeserializeFixture.h>
#include <CommonTestUtils/XMLSerializeFixture.h>
#include <rttr/registration>


struct GUIDWrapperStruct {
	GLEngine::GUID ID;
};
// clang-format off
RTTR_REGISTRATION
{
	rttr::registration::class_<GUIDWrapperStruct>("GUIDWrapperStruct")
		.constructor<>()
		.property("ID", &GUIDWrapperStruct::ID);
}
// clang-format on

namespace GLEngine {

TEST_F(XMLDeserializeFixture, GUID)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<GUIDWrapperStruct>
			<ID ID="0f1102d5-de01-4062-a0ae-35ab6d93c561" />
</GUIDWrapperStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<GUIDWrapperStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(result->get()->ID.toString(), "0f1102d5-de01-4062-a0ae-35ab6d93c561");
}

TEST_F(XMLDeserializeFixture, InvalidGUID)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<GUIDWrapperStruct>
			<ID ID="00000000-0000-0000-0000-000000000000" />
</GUIDWrapperStruct>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<GUIDWrapperStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	EXPECT_FALSE(result->get()->ID.isValid());
	EXPECT_EQ(result->get()->ID.toString(), "00000000-0000-0000-0000-000000000000");
}

TEST_F(XMLSerializeFixture, InvalidGUID)
{
	GUIDWrapperStruct container;
	container.ID = GUID::INVALID_GUID;

	const auto xmlDoc = serializer.Serialize(container);

	auto rootNode = xmlDoc.child("GUIDWrapperStruct");
	ASSERT_TRUE(rootNode);

	auto idNode = rootNode.child("ID");
	ASSERT_TRUE(idNode);

	EXPECT_STREQ("00000000-0000-0000-0000-000000000000", idNode.attribute("ID").value());
}

} // namespace GLEngine