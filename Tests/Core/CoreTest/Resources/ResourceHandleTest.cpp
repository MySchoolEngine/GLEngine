#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceHandle.h>

#include <CoreTest/Resources/TestResource.h>
#include <CoreTest/Resources/TestResource2.h>

namespace GLEngine::Core {

class ResourceHandleFixture : public ::testing::Test {
public:
	template <class ResourceType> static ResourceHandle<ResourceType> CreateResourceHandle(std::shared_ptr<ResourceType> resource)
	{
		resource->m_State = ResourceState::Ready;
		return ResourceHandle<ResourceType>(resource);
	}
};

TEST_F(ResourceHandleFixture, DefaultConstruct)
{
	ResourceHandle<TestResource> handle;
	EXPECT_FALSE(handle.IsReady());
}

TEST_F(ResourceHandleFixture, IsValid)
{
	ResourceHandle<TestResource> handle = CreateResourceHandle<TestResource>(std::make_shared<TestResource>());
	EXPECT_TRUE(handle.IsReady());
}

TEST_F(ResourceHandleFixture, EqualityWithSameResource)
{
	auto resource = std::make_shared<TestResource>();
	ResourceHandle<TestResource> handle1 = CreateResourceHandle<TestResource>(resource);
	ResourceHandle<TestResource> handle2 = CreateResourceHandle<TestResource>(resource);

	EXPECT_EQ(handle1, handle2);
}

} // namespace GLEngine::Core
