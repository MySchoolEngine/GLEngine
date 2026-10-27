#include <UtilsTestStdafx.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/XMLDeserialize.h>

#include <CommonTestUtils/XMLDeserializeFixture.h>

namespace GLEngine::Utils {

namespace {
// Mirrors how a component is actually handed to DeserializeInto in production: WorldSerializer::Load
// calls C_ComponentRegistry::Entry::get(), which returns std::ref(existingComponent) - a variant
// wrapping a std::reference_wrapper<T> that aliases a caller-owned object, not a freshly
// type.create()'d one. DeserializeDoc's own tests (XMLDeserializeTest.cpp) never exercise this path.
struct S_AfterDeserializeProbe {
	int	 value				  = 0;
	bool afterDeserializeRan = false;

	void AfterDeserialize() { afterDeserializeRan = true; }
};
} // namespace

} // namespace GLEngine::Utils

RTTR_REGISTRATION
{
	rttr::registration::class_<GLEngine::Utils::S_AfterDeserializeProbe>("AfterDeserializeProbe")
		.constructor<>()
		.method("AfterDeserialize", &GLEngine::Utils::S_AfterDeserializeProbe::AfterDeserialize)
		.property("value", &GLEngine::Utils::S_AfterDeserializeProbe::value);
}

namespace GLEngine::Utils {

// F-134 regression: DeserializeInto used to look AfterDeserialize up on the reference_wrapper<T>
// type itself (which has no registered methods), so this hook silently never ran.
TEST_F(XMLDeserializeFixture, DeserializeInto_OnAnExistingRefWrappedInstance_RunsAfterDeserialize)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<AfterDeserializeProbe value="42" />)x");

	S_AfterDeserializeProbe existing;
	ASSERT_FALSE(existing.afterDeserializeRan);

	rttr::variant var = std::ref(existing);
	deserializer.DeserializeInto(doc.root().first_child(), var);

	EXPECT_EQ(existing.value, 42);
	EXPECT_TRUE(existing.afterDeserializeRan);
}

} // namespace GLEngine::Utils
