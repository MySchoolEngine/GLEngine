#include <CoreTestStdafx.h>

#include <Utils/Serialization/SerializationUtils.h>

#include <CommonTestUtils/XMLSerializeFixture.h>

#include <CoreTest/Resources/TestResource.h>
#include <rttr/registration>

namespace GLEngine::Core {

class ResourceHandleContainer {
public:
	ResourceHandleContainer() = default;

	ResourceHandle<TestResource> handle;
	std::string					 containerName;

	RTTR_ENABLE()
};

} // namespace GLEngine::Core

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Core;

	rttr::registration::class_<ResourceHandleContainer>("ResourceHandleContainer")
		.constructor<>()
		.property("handle", &ResourceHandleContainer::handle)
		(rttr::policy::prop::as_reference_wrapper,
		REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<TestResource>()))
		.property("containerName", &ResourceHandleContainer::containerName);
}
// clang-format on

namespace GLEngine::Core {

/**
 * Test suite for ResourceHandle XML serialization.
 * Tests various scenarios including empty handles, handles with allocated resources,
 * and handles within container objects.
 */
class SerializeResourceHandle : public XMLSerializeFixture {
};

TEST_F(SerializeResourceHandle, Empty)
{
	ResourceHandle<TestResource> Handle;
	const auto					 xmlDoc = serializer.Serialize(Handle);

	auto rootNode = xmlDoc.child("TestResourceHandle");
	ASSERT_TRUE(rootNode);

	auto resourceNode = rootNode.child("Resource");
	ASSERT_TRUE(resourceNode);

	EXPECT_FALSE(resourceNode.attribute("derivedTypeCast"));
	EXPECT_FALSE(resourceNode.attribute("FilePath"));
}

TEST_F(SerializeResourceHandle, WithResource)
{
	auto testResource = std::make_shared<TestResource>();
	testResource->Load(std::filesystem::path("test/path/resource.dat"));
	ResourceHandle<TestResource> Handle(std::move(testResource));

	const auto xmlDoc = serializer.Serialize(Handle);

	auto rootNode = xmlDoc.child("TestResourceHandle");
	ASSERT_TRUE(rootNode);

	auto resourceNode = rootNode.child("Resource");
	ASSERT_TRUE(resourceNode);

	EXPECT_STREQ("GLEngine::Core::TestResource", resourceNode.attribute("derivedTypeCast").value());
	EXPECT_STREQ("test/path/resource.dat", resourceNode.attribute("FilePath").value());
}

TEST_F(SerializeResourceHandle, Container)
{
	auto testResource = std::make_shared<TestResource>();
	testResource->Load(std::filesystem::path("test/path/resource.dat"));
	ResourceHandle<TestResource> Handle(std::move(testResource));

	ResourceHandleContainer container;
	container.handle = Handle;

	const auto xmlDoc = serializer.Serialize(container);

	auto rootNode = xmlDoc.child("ResourceHandleContainer");
	ASSERT_TRUE(rootNode) << ToString(xmlDoc);

	auto handleNode = rootNode.child("handle");
	ASSERT_TRUE(handleNode);

	auto resourceNode = handleNode.child("Resource");
	ASSERT_TRUE(resourceNode);

	EXPECT_STREQ("GLEngine::Core::TestResource", resourceNode.attribute("derivedTypeCast").value());
	EXPECT_STREQ("test/path/resource.dat", resourceNode.attribute("FilePath").value());
}

TEST_F(SerializeResourceHandle, DefaultHandle)
{
	ResourceHandleContainer container;
	container.handle = {};
	container.containerName = "Container";

	const auto xmlDoc = serializer.Serialize(container);

	auto rootNode = xmlDoc.child("ResourceHandleContainer");
	ASSERT_TRUE(rootNode) << ToString(xmlDoc);

	EXPECT_STREQ("Container", rootNode.attribute("containerName").value());
	EXPECT_FALSE(rootNode.child("handle"));
}

} // namespace GLEngine::Core