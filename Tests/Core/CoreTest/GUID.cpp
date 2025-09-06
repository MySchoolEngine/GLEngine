#include <Core/GUID.h>

#include <CommonTestUtils/XMLDeserializeFixture.h>
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

} // namespace GLEngine