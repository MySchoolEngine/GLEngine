#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestClasses/DelayTestResource.h>
#include <CoreTest/Resources/Fixtures/ResourceManagerBaseFixture.h>

namespace GLEngine::Core {

class ResourceHandleFixture : public ResourceManagerBaseFixture {
public:
};

TEST_F(ResourceHandleFixture, DefaultConstruct)
{
	ResourceHandle<DelayTestResource> handle;
	EXPECT_FALSE(handle.IsReady());
}

TEST_F(ResourceHandleFixture, IsValid)
{
	ResourceHandle<DelayTestResource> handle = CreateResourceHandle<DelayTestResource>(std::make_shared<DelayTestResource>());
	EXPECT_TRUE(handle.IsReady());
}

TEST_F(ResourceHandleFixture, EqualityWithSameResource)
{
	auto						 resource = std::make_shared<DelayTestResource>();
	ResourceHandle<DelayTestResource> handle1  = CreateResourceHandle<DelayTestResource>(resource);
	ResourceHandle<DelayTestResource> handle2  = CreateResourceHandle<DelayTestResource>(resource);

	EXPECT_EQ(handle1, handle2);
}

} // namespace GLEngine::Core
