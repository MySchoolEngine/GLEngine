#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestResource.h>
#include <CoreTest/Resources/TestResource2.h>

namespace GLEngine::Core {

class ResourceManagerFixture : public ::testing::Test {
public:
	// Test file paths
	static inline const std::filesystem::path testPathTest2 = "test_resource.test2";
	static inline const std::filesystem::path testPathTest = "test_resource.test";

	void SetUp() override
	{
		// Get singleton instance
		auto& manager = C_ResourceManager::Instance();
	}

	void TearDown() override
	{
		// Clean up resources after each test
		FlushAllUnused(C_ResourceManager::Instance());
	}

	// Helper methods to access private members
	static const I_ResourceLoader* GetLoaderForExt(const C_ResourceManager& manager, const std::string& ext)
	{
		return manager.GetLoaderForExt(ext);
	}

	static std::shared_ptr<Resource> GetResourcePtr(C_ResourceManager& manager, const std::filesystem::path& filepath)
	{
		return manager.GetResourcePtr(filepath);
	}

	static const C_Metafile* GetMetafile(const C_ResourceManager& manager, const std::filesystem::path& resource)
	{
		return manager.GetMetafile(resource);
	}

	static bool IsResourcesEmpty(const C_ResourceManager& manager)
	{
		return manager.m_Resources.empty();
	}

	static bool IsUnusedListEmpty(const C_ResourceManager& manager)
	{
		return manager.m_UnusedList.empty();
	}

	static bool IsFinishedLoadsEmpty(const C_ResourceManager& manager)
	{
		return manager.m_FinishedLoads.empty();
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

// TODO: Implement tests once resource loaders are set up
// Test ideas:
// - LoadResource: Load a resource and verify it's cached
// - GetResource: Retrieve already loaded resource
// - UnloadUnusedResources: Verify unused resources are cleaned up
// - RegisterResourceType: Register custom loader and load resource with it
// - GetSupportedExtensions: Verify registered extensions

TEST_F(ResourceManagerFixture, AddingLoaders)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResource2Loader);
	EXPECT_NE(GetLoaderForExt(manager, ".test2"), nullptr);
	EXPECT_EQ(GetLoaderForExt(manager, ".not-existent"), nullptr);
}

TEST_F(ResourceManagerFixture, LoadResourceWithWrongLoader)
{
	auto& manager = C_ResourceManager::Instance();
	EXPECT_TRUE(IsResourcesEmpty(manager));
	EXPECT_TRUE(IsUnusedListEmpty(manager));
	EXPECT_TRUE(IsFinishedLoadsEmpty(manager));

	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestResourceLoader);

	const auto handle = manager.LoadResource<TestResource2>(testPathTest, true);
	EXPECT_FALSE(handle.IsReady());
	EXPECT_TRUE(handle.IsFailed());
}

TEST_F(ResourceManagerFixture, LoadResourceCorrectly)
{
	auto& manager = C_ResourceManager::Instance();
	EXPECT_TRUE(IsResourcesEmpty(manager));
	EXPECT_TRUE(IsUnusedListEmpty(manager));
	EXPECT_TRUE(IsFinishedLoadsEmpty(manager));

	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestResourceLoader);

	const auto handle = manager.LoadResource<TestResource2>(testPathTest2, true);
	EXPECT_TRUE(handle.IsReady());

	// Verify resource is cached
	auto cachedResource = GetResourcePtr(manager, testPathTest2);
	EXPECT_NE(cachedResource, nullptr);

	const auto handle1 = manager.GetResource<TestResource2>(testPathTest2);
	EXPECT_EQ(handle, handle1);
}

TEST_F(ResourceManagerFixture, LoadAlreadyLoadedResourceWithWrongType)
{
	auto& manager = C_ResourceManager::Instance();
	EXPECT_TRUE(IsResourcesEmpty(manager));
	EXPECT_TRUE(IsUnusedListEmpty(manager));
	EXPECT_TRUE(IsFinishedLoadsEmpty(manager));

	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestResourceLoader);

	// First load with correct type
	const auto handle = manager.LoadResource<TestResource2>(testPathTest2, true);
	EXPECT_TRUE(handle.IsReady());

	// Try to load the same resource with wrong type
	const auto handleWrongType = manager.LoadResource<TestResource>(testPathTest2, true);
	EXPECT_FALSE(handleWrongType.IsReady());
	EXPECT_TRUE(handleWrongType.IsFailed());
}

TEST_F(ResourceManagerFixture, UnloadUnusedResourcesAfterScopeExit)
{
	auto& manager = C_ResourceManager::Instance();
	EXPECT_TRUE(IsResourcesEmpty(manager));
	EXPECT_TRUE(IsUnusedListEmpty(manager));
	EXPECT_TRUE(IsFinishedLoadsEmpty(manager));

	manager.RegisterResourceType(new TestResource2Loader);

	{
		// Load resource in inner scope
		const auto handle = manager.LoadResource<TestResource2>(testPathTest2, true);
		EXPECT_TRUE(handle.IsReady());
		EXPECT_FALSE(IsResourcesEmpty(manager));

		// Handle goes out of scope here
	}
	EXPECT_FALSE(IsResourcesEmpty(manager));
	manager.UnloadUnusedResources();
	EXPECT_FALSE(IsResourcesEmpty(manager));

	// After scope exit, handle is destroyed, resource should be unused
	FlushAllUnused(manager);
	EXPECT_TRUE(IsResourcesEmpty(manager));
}

} // namespace GLEngine::Core
