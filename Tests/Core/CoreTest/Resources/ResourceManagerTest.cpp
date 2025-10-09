#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>
#include <Core/Resources/Metafile.h>

#include <CoreTest/Resources/TestResource.h>
#include <CoreTest/Resources/TestResource2.h>
#include <CoreTest/Resources/TestResourceBuildable.h>

namespace GLEngine::Core {

class ResourceManagerFixture : public ::testing::Test {
public:
	// Test file paths
	static inline const std::filesystem::path testPathTest2 = "test_resource.test2";
	static inline const std::filesystem::path testPathTest = "test_resource.test";
	static inline const std::filesystem::path testPathTestBuildable = "test_resource.testbuild";

	void SetUp() override
	{
		// Get singleton instance
		auto& manager = C_ResourceManager::Instance();
		EXPECT_TRUE(IsResourcesEmpty(manager));
		EXPECT_TRUE(IsUnusedListEmpty(manager));
		EXPECT_TRUE(IsFinishedLoadsEmpty(manager));
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

	static C_Metafile& GetOrCreateMetafile(C_ResourceManager& manager, const std::filesystem::path& resource)
	{
		return manager.GetOrCreateMetafile(resource);
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

TEST_F(ResourceManagerFixture, AddingLoaders)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResource2Loader);
	EXPECT_NE(GetLoaderForExt(manager, ".test2"), nullptr) << "Loader for .test2 extension should be registered";
	EXPECT_EQ(GetLoaderForExt(manager, ".not-existent"), nullptr) << "Loader for non-existent extension should return nullptr";
}

TEST_F(ResourceManagerFixture, GetSupportedExtensions)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResourceLoader);
	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestResourceBuildableLoader);

	// Get extensions for TestResource
	const auto testExts = manager.GetSupportedExtensions(TestResource::GetResourceTypeHashStatic());
	EXPECT_EQ(testExts.size(), 2) << "TestResource should support 2 extensions";
	EXPECT_TRUE(std::find(testExts.begin(), testExts.end(), ".test") != testExts.end()) << "TestResource should support .test extension";
	EXPECT_TRUE(std::find(testExts.begin(), testExts.end(), ".test-slow") != testExts.end()) << "TestResource should support .test-slow extension";

	// Get extensions for TestResource2
	const auto test2Exts = manager.GetSupportedExtensions(TestResource2::GetResourceTypeHashStatic());
	EXPECT_EQ(test2Exts.size(), 2) << "TestResource2 should support 2 extensions";
	EXPECT_TRUE(std::find(test2Exts.begin(), test2Exts.end(), ".test2") != test2Exts.end()) << "TestResource2 should support .test2 extension";
	EXPECT_TRUE(std::find(test2Exts.begin(), test2Exts.end(), ".test2-2") != test2Exts.end()) << "TestResource2 should support .test2-2 extension";

	// Get extensions for TestResourceBuildable
	const auto buildableExts = manager.GetSupportedExtensions(TestResourceBuildable::GetResourceTypeHashStatic());
	EXPECT_EQ(buildableExts.size(), 1) << "TestResourceBuildable should support 1 extension";
	EXPECT_EQ(buildableExts[0], ".testbuild") << "TestResourceBuildable should support .testbuild extension";
}

TEST_F(ResourceManagerFixture, LoadResourceWithWrongLoader)
{
	auto& manager = C_ResourceManager::Instance();

	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestResourceLoader);

	const auto handle = manager.LoadResource<TestResource2>(testPathTest, true);
	EXPECT_FALSE(handle.IsReady()) << "Loading .test file as TestResource2 should fail (wrong loader)";
	EXPECT_TRUE(handle.IsFailed()) << "Handle should be in failed state when loaded with wrong type";
}

TEST_F(ResourceManagerFixture, LoadResourceCorrectly)
{
	auto& manager = C_ResourceManager::Instance();

	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestResourceLoader);

	const auto handle = manager.LoadResource<TestResource2>(testPathTest2, true);
	EXPECT_TRUE(handle.IsReady()) << "TestResource2 should load successfully from .test2 file";

	// Verify resource is cached
	auto cachedResource = GetResourcePtr(manager, testPathTest2);
	EXPECT_NE(cachedResource, nullptr) << "Loaded resource should be cached in manager";

	const auto handle1 = manager.GetResource<TestResource2>(testPathTest2);
	EXPECT_EQ(handle, handle1) << "GetResource should return the same handle as LoadResource for already loaded resource";
}

TEST_F(ResourceManagerFixture, LoadAlreadyLoadedResourceWithWrongType)
{
	auto& manager = C_ResourceManager::Instance();

	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestResourceLoader);

	// First load with correct type
	const auto handle = manager.LoadResource<TestResource2>(testPathTest2, true);
	EXPECT_TRUE(handle.IsReady()) << "First load with correct type should succeed";

	// Try to load the same resource with wrong type
	const auto handleWrongType = manager.LoadResource<TestResource>(testPathTest2, true);
	EXPECT_FALSE(handleWrongType.IsReady()) << "Loading already loaded resource with wrong type should fail";
	EXPECT_TRUE(handleWrongType.IsFailed()) << "Handle should be in failed state when type mismatch occurs";
}

TEST_F(ResourceManagerFixture, UnloadUnusedResourcesAfterScopeExit)
{
	auto& manager = C_ResourceManager::Instance();

	manager.RegisterResourceType(new TestResource2Loader);

	{
		// Load resource in inner scope
		const auto handle = manager.LoadResource<TestResource2>(testPathTest2, true);
		EXPECT_TRUE(handle.IsReady()) << "Resource should load successfully in inner scope";
		EXPECT_FALSE(IsResourcesEmpty(manager)) << "Resource should be cached while handle exists";

		// Handle goes out of scope here
	}
	EXPECT_FALSE(IsResourcesEmpty(manager)) << "Resource should still be cached immediately after handle destruction";
	manager.UnloadUnusedResources();
	EXPECT_FALSE(IsResourcesEmpty(manager)) << "Resource should still exist after first UnloadUnusedResources call (delayed cleanup)";

	// After scope exit, handle is destroyed, resource should be unused
	FlushAllUnused(manager);
	EXPECT_TRUE(IsResourcesEmpty(manager)) << "Resource should be removed after full flush of unused resources";
}

TEST_F(ResourceManagerFixture, LoadResourceAsync)
{
	auto& manager = C_ResourceManager::Instance();

	manager.RegisterResourceType(new TestResourceLoader);

	// Load resource asynchronously (non-blocking)
	const auto handle = manager.LoadResource<TestResource>(testPathTest, false);

	// Right after load request, should be loading
	EXPECT_TRUE(handle.IsLoading()) << "Handle should be in loading state immediately after async load request";
	EXPECT_FALSE(handle.IsReady()) << "Handle should not be ready immediately after async load request";

	// Wait for load to complete (TestResource has 100ms delay)
	std::this_thread::sleep_for(std::chrono::milliseconds(120));

	// Process finished loads
	manager.UpdatePendingLoads();

	// Now should be ready
	EXPECT_TRUE(handle.IsReady()) << "Handle should be ready after async load completes and UpdatePendingLoads is called";
	EXPECT_FALSE(handle.IsLoading()) << "Handle should no longer be in loading state after load completes";
}

TEST_F(ResourceManagerFixture, LoadBuildableResource)
{
	auto& manager = C_ResourceManager::Instance();

	manager.RegisterResourceType(new TestResourceLoader);
	manager.RegisterResourceType(new TestResourceBuildableLoader);

	// Load buildable resource (it should build from base resource)
	const auto handleTest	   = manager.LoadResource<TestResource>(testPathTest, true);
	const auto handleBuildable = manager.LoadResource<TestResourceBuildable>(testPathTestBuildable, true);

	// Verify it was built successfully
	EXPECT_TRUE(handleBuildable.IsReady()) << "Buildable resource should be ready after building from base resource";
	EXPECT_FALSE(handleBuildable.IsFailed()) << "Buildable resource should not be in failed state";

	// Verify built data (TestResource has testData=42, buildable doubles it)
	auto& buildable = handleBuildable.GetResource();
	EXPECT_EQ(buildable.builtData, 84) << "Built data should be 84 (base testData 42 * 2)"; // 42 * 2
	EXPECT_EQ(buildable.builtName, "Built_DefaultTest") << "Built name should be prefixed with 'Built_'";
}

TEST_F(ResourceManagerFixture, LoadBuildableResourceWithWrongExtension)
{
	auto& manager = C_ResourceManager::Instance();

	manager.RegisterResourceType(new TestResourceLoader);
	manager.RegisterResourceType(new TestResourceBuildableLoader);

	// Try to load TestResourceBuildable with .test extension (wrong loader)
	const auto handle = manager.LoadResource<TestResourceBuildable>(testPathTest, true);

	// Should fail because .test extension is for TestResource, not TestResourceBuildable
	EXPECT_FALSE(handle.IsReady()) << "Loading TestResourceBuildable with .test extension should fail (wrong loader)";
	EXPECT_TRUE(handle.IsFailed()) << "Handle should be in failed state when extension doesn't match resource type";
}

} // namespace GLEngine::Core
