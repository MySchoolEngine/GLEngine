#include <CoreTestStdafx.h>

#include <CoreTest/Resources/Fixtures/ResourceManagerBaseFixture.h>
#include <CoreTest/Resources/TestClasses/TestResourceWithPropertyFile.h>

namespace GLEngine::Core {


static const inline std::filesystem::path s_TestFilepath{"ResourceLoadSaveFixture"};
static inline const std::filesystem::path testPathTest{"test_resource.test"};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class ResourceLoadSaveFixture : public ResourceManagerBaseFixture {
public:
	void TearDown() override
	{
		RemoveMetafileIfExists(testPathTest);
		ResourceManagerBaseFixture::TearDown();
	}
};

TEST_F(ResourceLoadSaveFixture, SaveDummyEmptyInner)
{
	auto	   resource = std::make_shared<TestResourceWithPropertyFile>();
	const auto filePath = s_TestFilepath / "dummyEmptySave.fileprop";
	DeleteOnTearDown(filePath);

	SetFilePath(resource, filePath);
	SetDirty(resource);
	EXPECT_TRUE(resource->Save());
}

TEST_F(ResourceLoadSaveFixture, SaveDummyFilledInner)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new DelayTestResourceLoader);
	const auto filePath = s_TestFilepath / "dummyFilledSave.fileprop";
	DeleteOnTearDown(filePath);

	auto	   resource			   = std::make_shared<TestResourceWithPropertyFile>();
	const auto handleTestResource2 = manager.LoadResource<DelayTestResource>(testPathTest, true);
	resource->m_InnerHandle		   = handleTestResource2;
	SetFilePath(resource, filePath);
	SetDirty(resource);
	EXPECT_TRUE(resource->Save());
}

TEST_F(ResourceLoadSaveFixture, LoadDummyFilledInner_Instant)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new DelayTestResourceLoader);
	manager.RegisterResourceType(new TestResourceWithPropertyFileLoader);
	const auto filePath = s_TestFilepath / "dummyFilledInstantLoad.fileprop";

	const auto handleLoaded = manager.LoadResource<TestResourceWithPropertyFile>(filePath, true);
	EXPECT_TRUE(handleLoaded.IsReady());
	const auto innerHandle = manager.GetResource<DelayTestResource>(testPathTest);
	EXPECT_TRUE(innerHandle.IsReady());

	// cleanup
	RemoveMetafileIfExists(filePath);
}

TEST_F(ResourceLoadSaveFixture, LoadDummyFilledInner_Delayed)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new DelayTestResourceLoader);
	manager.RegisterResourceType(new TestResourceWithPropertyFileLoader);
	const auto filePath = s_TestFilepath / "dummyFilledInstantLoad.fileprop";

	const auto handleLoaded = manager.LoadResource<TestResourceWithPropertyFile>(filePath, false);
	EXPECT_FALSE(handleLoaded.IsFailed());
	EXPECT_FALSE(handleLoaded.IsReady());

	while (true)
	{
		manager.UpdatePendingLoads();
		const auto innerHandle = manager.GetResource<DelayTestResource>(testPathTest);
		if (innerHandle.IsFailed() == false)
		{
			EXPECT_TRUE(innerHandle.IsLoading());
			EXPECT_FALSE(handleLoaded.IsReady());
			break;
		}
	}
	std::this_thread::sleep_for(DelayTestResource::s_LoadTime * 2);
	manager.UpdatePendingLoads();
	const auto innerHandle = manager.GetResource<DelayTestResource>(testPathTest);
	EXPECT_TRUE(innerHandle.IsReady());
	EXPECT_TRUE(handleLoaded.IsReady());


	// cleanup
	RemoveMetafileIfExists(filePath);
}

} // namespace GLEngine::Core
