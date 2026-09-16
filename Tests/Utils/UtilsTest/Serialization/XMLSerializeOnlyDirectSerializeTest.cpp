#include <UtilsTestStdafx.h>
#include <gtest/gtest.h>

#include <Utils/Serialization/XMLSerialize.h>

#include <CommonTestUtils/XMLSerializeFixture.h>

#include <UtilsTest/Serialization/DummyStructs_Polymorphic.h>
#include <memory>

namespace GLEngine::Utils {

TEST_F(XMLSerializeFixture, OnlyDirectSerialize_RootInstance_SerializesAllProperties)
{
	auto resource		= std::make_shared<DerivedResourceLike>();
	resource->filePath	= "Models\\some.res";
	resource->cache		= "cached-data";
	resource->extraData = 3.5f;

	const auto xmlDoc	= serializer.Serialize(resource);
	const auto rootNode = xmlDoc.root().first_child();

	EXPECT_EQ(rootNode.name(), std::string("DerivedResourceLike")) << ToString(xmlDoc);
	EXPECT_STREQ("Models\\some.res", rootNode.attribute("filePath").value()) << ToString(xmlDoc);
	EXPECT_STREQ("cached-data", rootNode.attribute("cache").value()) << ToString(xmlDoc);
	EXPECT_FLOAT_EQ(3.5f, rootNode.attribute("extraData").as_float()) << ToString(xmlDoc);
}

TEST_F(XMLSerializeFixture, OnlyDirectSerialize_NestedInstance_OnlySerializesAlwaysSerializeProperties)
{
	auto resource		= std::make_shared<DerivedResourceLike>();
	resource->filePath	= "Models\\some.res";
	resource->cache		= "cached-data";
	resource->extraData = 3.5f;

	ResourceHandleLikeContainer container;
	container.resource = resource;

	const auto xmlDoc	= serializer.Serialize(container);
	const auto rootNode = xmlDoc.root().first_child();
	EXPECT_EQ(rootNode.name(), std::string("ResourceHandleLikeContainer")) << ToString(xmlDoc);

	const auto resourceNode = rootNode.child("resource");
	ASSERT_TRUE(resourceNode) << ToString(xmlDoc);

	// filePath is tagged AlwaysSerialize, so it must survive even though the owner is nested.
	EXPECT_STREQ("Models\\some.res", resourceNode.attribute("filePath").value()) << ToString(xmlDoc);

	// cache (declared on the OnlyDirectSerialize base) and extraData (declared on the derived
	// type) are both untagged, so they must be dropped when not the document root.
	EXPECT_FALSE(resourceNode.attribute("cache")) << ToString(xmlDoc);
	EXPECT_FALSE(resourceNode.attribute("extraData")) << ToString(xmlDoc);

	// Polymorphic dispatch still works normally for nested OnlyDirectSerialize instances.
	EXPECT_STREQ("DerivedResourceLike", resourceNode.attribute("derivedTypeCast").value()) << ToString(xmlDoc);
}

} // namespace GLEngine::Utils
