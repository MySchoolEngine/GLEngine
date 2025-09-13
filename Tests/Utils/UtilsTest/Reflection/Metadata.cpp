#include <UtilsTestStdafx.h>

#include <Utils/Reflection/Metadata.h>
#include <CommonTestUtils/XMLDeserializeFixture.h>

struct MetadataTestStruct {
	float angle;
};
// clang-format off
RTTR_REGISTRATION
{
	using namespace Utils::Reflection;
	rttr::registration::class_<MetadataTestStruct>("MetadataTestStruct")
		.constructor<>()
		.property("angle", &MetadataTestStruct::angle)
			(
				rttr::policy::prop::as_reference_wrapper,
				RegisterMetaclass<MetaGUI::Angle>(),
				RegisterMetamember<UI::Angle::Name>("angle"),
				RegisterMetamember<UI::Angle::Min>(-89.0f),
				RegisterMetamember<UI::Angle::Max>(89.0f)
			 );
}
// clang-format on

namespace GLEngine::Utils {
TEST_F(XMLDeserializeFixture, AtomicAsReferenceWrapper)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<MetadataTestStruct angle="10">
</MetadataTestStruct>
)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<MetadataTestStruct>>(doc);
	EXPECT_TRUE(result.has_value());
	EXPECT_EQ(result.value()->angle, 10.f);
}
} // namespace GLEngine::Utils