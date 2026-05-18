#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestClasses/DelayTestResource.h>
#include <CoreTest/Resources/TestClasses/TestResource2.h>

namespace GLEngine::Core {

class ResourceHandleFixture : public ::testing::Test {
public:
	template <class ResourceType> static ResourceHandle<ResourceType> CreateResourceHandle(std::shared_ptr<ResourceType> resource)
	{
		resource->m_State = ResourceState::Ready;
		return ResourceHandle<ResourceType>(resource);
	}

	void TearDown() override
	{
		auto& manager = C_ResourceManager::Instance();
		// Clean up resources after each test
		FlushAllUnused(manager);
	}

	/**
	 * @brief Calls UnloadUnusedResources s_UpdatesBeforeDelete times to fully flush all unused resources.
	 */
	static void FlushAllUnused(C_ResourceManager& manager)
	{
		for (unsigned int i = 0; i <= C_ResourceManager::s_UpdatesBeforeDelete; ++i)
		{
			manager.UnloadUnusedResources();
		}
	}
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
