#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

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
	auto						 resource = std::make_shared<TestResource>();
	ResourceHandle<TestResource> handle1  = CreateResourceHandle<TestResource>(resource);
	ResourceHandle<TestResource> handle2  = CreateResourceHandle<TestResource>(resource);

	EXPECT_EQ(handle1, handle2);
}

} // namespace GLEngine::Core
