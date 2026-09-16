#include <UtilsTestStdafx.h>
#include <gtest/gtest.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <CommonTestUtils/XMLDeserializeFixture.h>

#include <pugixml.hpp>

#include <UtilsTest/Serialization/DummyStructs_Polymorphic.h>

namespace GLEngine::Utils {

TEST_F(XMLDeserializeFixture, OnlyDirectSerialize_RootInstance_DeserializesAllProperties)
{
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<DerivedResourceLike filePath="Models\some.res" cache="cached-data" extraData="3.5" />)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<DerivedResourceLike>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& resource = result.value();
	ASSERT_TRUE(resource);
	EXPECT_EQ(resource->filePath, "Models\\some.res");
	EXPECT_EQ(resource->cache, "cached-data");
	EXPECT_FLOAT_EQ(resource->extraData, 3.5f);
}

TEST_F(XMLDeserializeFixture, OnlyDirectSerialize_NestedInstance_FilePathOnly_Deserializes)
{
	// Matches what the serializer now writes for a nested OnlyDirectSerialize resource: only
	// the AlwaysSerialize-tagged property (filePath) is present.
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<ResourceHandleLikeContainer>
	<resource derivedTypeCast="DerivedResourceLike" filePath="Models\some.res" />
</ResourceHandleLikeContainer>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<ResourceHandleLikeContainer>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& container = result.value();
	ASSERT_TRUE(container->resource);
	EXPECT_EQ(container->resource->filePath, "Models\\some.res");
	EXPECT_EQ(container->resource->cache, "") << "cache was never read for a nested instance, so it stays default-constructed";

	const auto derived = std::dynamic_pointer_cast<DerivedResourceLike>(container->resource);
	ASSERT_TRUE(derived);
	EXPECT_FLOAT_EQ(derived->extraData, 0.0f) << "extraData was never read for a nested instance, so it stays default-constructed";
}

TEST_F(XMLDeserializeFixture, OnlyDirectSerialize_NestedInstance_IgnoresStaleNonAlwaysSerializeAttributes)
{
	// Simulates an old file saved before this feature existed, which still has cache/extraData
	// embedded even though the resource is nested. They must be ignored on load, not just absent
	// on save - the skip happens in the read-side property loop regardless of what's in the XML.
	const pugi::xml_document doc = ConstructDocument(R"x(
<?xml version="1.0"?>
<ResourceHandleLikeContainer>
	<resource derivedTypeCast="DerivedResourceLike" filePath="Models\some.res" cache="STALE_SHOULD_BE_IGNORED" extraData="99" />
</ResourceHandleLikeContainer>)x");

	const auto result = deserializer.Deserialize<std::shared_ptr<ResourceHandleLikeContainer>>(doc);
	ASSERT_TRUE(result.has_value());

	const auto& container = result.value();
	ASSERT_TRUE(container->resource);
	EXPECT_EQ(container->resource->filePath, "Models\\some.res");
	EXPECT_EQ(container->resource->cache, "");

	const auto derived = std::dynamic_pointer_cast<DerivedResourceLike>(container->resource);
	ASSERT_TRUE(derived);
	EXPECT_FLOAT_EQ(derived->extraData, 0.0f);
}

} // namespace GLEngine::Utils
