#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestResource2.h>
#include <CoreTest/Resources/TestResourceWithProperty.h>

#include <thread>

//todo delete meta files

// Tests that cover the scenario where an outer resource contains another
// resource as a property, and the outer resource must not be considered Ready
// until the inner property resource has also finished loading.
//
// Two sub-scenarios are exercised:
//
//   1. Sync property loading (outer loaded blocking=true):
//      Load() is called on the main thread. It loads the inner resource
//      synchronously. The outer handle is Ready only after both have finished.
//
//   2. Async property loading (outer loaded blocking=false):
//      Load() runs on a background thread. Inside Load(), the inner resource
//      is loaded synchronously (blocking the outer thread). The main thread
//      observes the outer handle in Loading state while both loads are in
//      flight, transitioning to Ready only after UpdatePendingLoads() is
//      called following thread completion.

namespace GLEngine::Core {

// ---------------------------------------------------------------------------
// Helper: a ResourceLoader for TestResourceWithProperty that injects the inner
// resource path into the freshly created resource object before the manager
// calls Load() on it. This is the only way to configure the resource without
// modifying production code, because LoadResource() creates the resource via
// the loader and immediately calls Load().
// ---------------------------------------------------------------------------
struct PropertyConfiguringLoader final : public ResourceLoader<TestResourceWithProperty> {
	std::filesystem::path innerResourcePath;

	std::shared_ptr<Resource> CreateResource() const override
	{
		auto res				= std::make_shared<TestResourceWithProperty>();
		res->m_InnerResourcePath = innerResourcePath;
		return res;
	}

	std::vector<std::string> GetSupportedExtensions() const override { return {".testprop"}; }
};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class ResourcePropertyLoadingFixture : public ::testing::Test {
public:
	static inline const std::filesystem::path outerPath = "outer_resource.testprop";
	static inline const std::filesystem::path innerPath = "inner_resource.test2";

	void SetUp() override
	{
		auto& manager = C_ResourceManager::Instance();
		VerifyManagerEmpty(manager, "SetUp");
	}

	void TearDown() override
	{
		auto& manager = C_ResourceManager::Instance();
		FlushAllUnused(manager);
		manager.Destroy();

		RemoveMetafileIfExists(outerPath);
		RemoveMetafileIfExists(innerPath);

		VerifyManagerEmpty(manager, "TearDown");
	}

	// -----------------------------------------------------------------------
	// Helpers
	// -----------------------------------------------------------------------

	static void FlushAllUnused(C_ResourceManager& manager)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		// s_UpdatesBeforeDelete is not enough as the outer first needs to expire, then the inner
		for (unsigned int i = 0; i <= C_ResourceManager::s_UpdatesBeforeDelete * C_ResourceManager::s_NumUpdatesBetweenUnloading * 2; ++i)
		{
			const auto numResourcesBefore = manager.m_Resources.size();
			manager.UpdatePendingLoads();
			if (manager.m_Resources.size() == 0)
				break;
			if (numResourcesBefore != manager.m_Resources.size())
				i = 0;
		}
	}

	static void RemoveMetafileIfExists(const std::filesystem::path& resourcePath)
	{
		std::error_code ec;
		std::filesystem::remove(C_Metafile::GetMetafileName(resourcePath), ec);
	}

	// Register a fresh set of loaders for each test, using the configuring
	// loader so the inner resource path is injected before Load() runs.
	static void RegisterLoaders(C_ResourceManager& manager, const std::filesystem::path& innerPath)
	{
		auto* configuringLoader		   = new PropertyConfiguringLoader;
		configuringLoader->innerResourcePath = innerPath;
		manager.RegisterResourceType(new TestResource2Loader);
		manager.RegisterResourceType(configuringLoader);
	}

	// Register loaders but point the inner path at an extension that has no
	// registered loader so the inner load will fail.
	static void RegisterLoadersWithBadInnerPath(C_ResourceManager& manager)
	{
		auto* configuringLoader		   = new PropertyConfiguringLoader;
		configuringLoader->innerResourcePath = "nonexistent_inner.unknown_ext";
		manager.RegisterResourceType(new TestResource2Loader);
		manager.RegisterResourceType(configuringLoader);
	}

	static void VerifyManagerEmpty(const C_ResourceManager& manager, const std::string& stage)
	{
		EXPECT_TRUE(manager.m_Resources.empty()) << stage << ": m_Resources should be empty";
		EXPECT_TRUE(manager.m_UnusedList.empty()) << stage << ": m_UnusedList should be empty";
		EXPECT_TRUE(manager.m_FinishedLoads.empty()) << stage << ": m_FinishedLoads should be empty";
		EXPECT_TRUE(manager.m_ExtToLoaders.empty()) << stage << ": m_ExtToLoaders should be empty";
		EXPECT_TRUE(manager.m_TypeIdToLoader.empty()) << stage << ": m_TypeIdToLoader should be empty";
	}
};

// =============================================================================
// 1. Synchronous property loading
// =============================================================================

// The outer resource is loaded synchronously (blocking=true). Its Load() calls
// the inner resource load synchronously. Both resources must be Ready when
// LoadResource returns.
TEST_F(ResourcePropertyLoadingFixture, SyncPropertyLoad_OuterReadyAfterInnerReady)
{
	auto& manager = C_ResourceManager::Instance();
	RegisterLoaders(manager, innerPath);

	const auto outerHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, true);

	EXPECT_TRUE(outerHandle.IsReady())
		<< "Outer resource should be Ready after sync load when inner property loads synchronously";
	EXPECT_FALSE(outerHandle.IsLoading())
		<< "Outer resource should not remain in Loading state after sync load";
	EXPECT_FALSE(outerHandle.IsFailed())
		<< "Outer resource should not be in Failed state when inner loads successfully";
}

// After a successful sync outer load, the inner property handle stored on the
// resource must also be in the Ready state.
TEST_F(ResourcePropertyLoadingFixture, SyncPropertyLoad_InnerHandleReadyOnOuterResource)
{
	// TODO This is failing and it is correct
	auto& manager = C_ResourceManager::Instance();
	RegisterLoaders(manager, innerPath);

	const auto outerHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, true);
	ASSERT_TRUE(outerHandle.IsReady());

	const auto& outerResource = outerHandle.GetResource();
	EXPECT_TRUE(outerResource.m_InnerHandle.IsReady())
		<< "Inner property resource handle must be Ready after the outer sync load completes";
	EXPECT_FALSE(outerResource.m_InnerHandle.IsFailed())
		<< "Inner property resource handle must not be Failed after successful load";
	EXPECT_FALSE(outerResource.m_InnerHandle.IsLoading())
		<< "Inner property resource handle must not remain in Loading after sync load";
}

// When the inner resource cannot be loaded (no registered loader for its
// extension), the outer resource's Load() returns false. The outer resource
// must end up in Failed state, not Ready.
TEST_F(ResourcePropertyLoadingFixture, SyncPropertyLoad_OuterFailedWhenInnerFails)
{
	//TODO This is failing and it is correct
	// It is questionable if I should really fail outer resource, if inner fails
	auto& manager = C_ResourceManager::Instance();
	RegisterLoadersWithBadInnerPath(manager);

	const auto outerHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, true);

	EXPECT_FALSE(outerHandle.IsReady())
		<< "Outer resource must not be Ready when the inner property resource fails to load";
	EXPECT_TRUE(outerHandle.IsFailed())
		<< "Outer resource must be in Failed state when the inner property resource fails";
}

// Loading the same outer resource a second time returns the cached handle and
// does not re-trigger the inner resource load.
TEST_F(ResourcePropertyLoadingFixture, SyncPropertyLoad_SecondLoadReturnsCachedHandle)
{
	auto& manager = C_ResourceManager::Instance();
	RegisterLoaders(manager, innerPath);

	const auto firstHandle  = manager.LoadResource<TestResourceWithProperty>(outerPath, true);
	const auto secondHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, true);

	EXPECT_TRUE(firstHandle.IsReady()) << "First load should succeed";
	EXPECT_EQ(firstHandle, secondHandle)
		<< "Second load of the same path should return the cached handle";
}

// =============================================================================
// 2. Asynchronous property loading
// =============================================================================

// The outer resource is loaded asynchronously (blocking=false). Its Load()
// internally loads the inner resource synchronously (blocking the background
// thread). The main thread must observe the outer handle in Loading state
// immediately after the async request.
TEST_F(ResourcePropertyLoadingFixture, AsyncPropertyLoad_OuterIsLoadingImmediatelyAfterRequest)
{
	auto& manager = C_ResourceManager::Instance();
	RegisterLoaders(manager, innerPath);

	const auto outerHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, false);

	EXPECT_TRUE(outerHandle.IsLoading())
		<< "Outer resource must be in Loading state immediately after an async load request";
	EXPECT_FALSE(outerHandle.IsReady())
		<< "Outer resource must not be Ready immediately after an async load request";
}

// After the async outer load completes (and UpdatePendingLoads is called), the
// outer resource must be Ready and the inner property handle must also be Ready.
TEST_F(ResourcePropertyLoadingFixture, AsyncPropertyLoad_OuterReadyAfterCompletion)
{
	auto& manager = C_ResourceManager::Instance();
	RegisterLoaders(manager, innerPath);

	const auto outerHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, false);

	// The outer thread loads the inner resource synchronously then returns.
	// TestResource2::Load() is nearly instantaneous, so a generous timeout
	// covers all scheduling overhead.
	constexpr int maxWaitMs	   = 500;
	constexpr int pollIntervalMs = 10;
	int			  elapsed		   = 0;
	while (outerHandle.IsLoading() && elapsed < maxWaitMs)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs));
		elapsed += pollIntervalMs;
		manager.UpdatePendingLoads();
	}
	// Final pump to ensure state is promoted.
	manager.UpdatePendingLoads();

	EXPECT_TRUE(outerHandle.IsReady())
		<< "Outer resource must be Ready after async load completes and UpdatePendingLoads is called";
	EXPECT_FALSE(outerHandle.IsLoading())
		<< "Outer resource must not remain in Loading state after async load finishes";
	EXPECT_FALSE(outerHandle.IsFailed())
		<< "Outer resource must not be in Failed state after successful async load";
}

// After the async outer load completes, the inner property handle stored on
// the resource must be in the Ready state. The outer remained Loading until
// the inner was settled.
TEST_F(ResourcePropertyLoadingFixture, AsyncPropertyLoad_InnerHandleReadyOnOuterResource)
{
	auto& manager = C_ResourceManager::Instance();
	RegisterLoaders(manager, innerPath);

	const auto outerHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, false);

	// Outer is Loading: inner has not yet been loaded from the main thread's
	// perspective because the outer's Load() thread has not finished.
	EXPECT_TRUE(outerHandle.IsLoading())
		<< "Outer resource must be Loading before the background thread finishes";

	constexpr int maxWaitMs	   = 500;
	constexpr int pollIntervalMs = 10;
	int			  elapsed		   = 0;
	while (outerHandle.IsLoading() && elapsed < maxWaitMs)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs));
		elapsed += pollIntervalMs;
		manager.UpdatePendingLoads();
	}
	manager.UpdatePendingLoads();

	ASSERT_TRUE(outerHandle.IsReady())
		<< "Outer resource must be Ready before inspecting the inner handle";

	const auto& outerResource = outerHandle.GetResource();
	EXPECT_TRUE(outerResource.m_InnerHandle.IsReady())
		<< "Inner property resource handle must be Ready after the outer async load completes";
	EXPECT_FALSE(outerResource.m_InnerHandle.IsLoading())
		<< "Inner property resource handle must not still be Loading after the outer finishes";
	EXPECT_FALSE(outerResource.m_InnerHandle.IsFailed())
		<< "Inner property resource handle must not be Failed after successful async load";
}

// When the inner resource fails to load during an async outer load, the outer's
// Load() detects the failure (inner.IsReady() == false) and returns false.
// After UpdatePendingLoads(), the outer must be in Failed state, not Ready.
TEST_F(ResourcePropertyLoadingFixture, AsyncPropertyLoad_OuterFailedWhenInnerFails)
{
	// TODO This is failing and it is correct
	auto& manager = C_ResourceManager::Instance();
	RegisterLoadersWithBadInnerPath(manager);

	const auto outerHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, false);

	EXPECT_TRUE(outerHandle.IsLoading())
		<< "Outer resource should start as Loading immediately after async request";

	constexpr int maxWaitMs	   = 500;
	constexpr int pollIntervalMs = 10;
	int			  elapsed		   = 0;
	while (outerHandle.IsLoading() && elapsed < maxWaitMs)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(pollIntervalMs));
		elapsed += pollIntervalMs;
		manager.UpdatePendingLoads();
	}
	manager.UpdatePendingLoads();

	EXPECT_FALSE(outerHandle.IsReady())
		<< "Outer resource must not be Ready when the inner property resource failed to load";
	EXPECT_TRUE(outerHandle.IsFailed())
		<< "Outer resource must be in Failed state when the inner property resource failed";
}

// Verify that the outer handle never becomes Ready before UpdatePendingLoads
// has been called after the background thread finishes — i.e., state promotion
// is gated on the main-thread pump.
TEST_F(ResourcePropertyLoadingFixture, AsyncPropertyLoad_OuterNotReadyBeforeUpdatePendingLoads)
{
	auto& manager = C_ResourceManager::Instance();
	RegisterLoaders(manager, innerPath);

	const auto outerHandle = manager.LoadResource<TestResourceWithProperty>(outerPath, false);

	// Wait for the background thread to finish (pushed to m_FinishedLoads) but
	// deliberately do NOT call UpdatePendingLoads yet.
	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	// Without UpdatePendingLoads the outer must still be in Loading state even
	// though the background thread has completed.
	EXPECT_FALSE(outerHandle.IsReady())
		<< "Outer resource must not be Ready before UpdatePendingLoads is called, "
		   "even after the background thread has finished";
	EXPECT_TRUE(outerHandle.IsLoading())
		<< "Outer resource must still be in Loading state until UpdatePendingLoads is called";

	// Now promote: outer becomes Ready.
	manager.UpdatePendingLoads();

	EXPECT_TRUE(outerHandle.IsReady())
		<< "Outer resource must be Ready after UpdatePendingLoads is called";
}

} // namespace GLEngine::Core
