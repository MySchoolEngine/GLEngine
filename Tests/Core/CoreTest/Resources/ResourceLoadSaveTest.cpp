#include <CoreTestStdafx.h>

#include <CoreTest/Resources/Fixtures/ResourceManagerBaseFixture.h>
#include <CoreTest/Resources/TestClasses/TestResourceWithPropertyFile.h>

namespace GLEngine::Core {


// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class ResourceLoadSaveFixture : public ResourceManagerBaseFixture {
public:
};

static const inline std::filesystem::path s_TestFilepath{"ResourceLoadSaveFixture"};
static inline const std::filesystem::path testPathTest{"test_resource.test2"};

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
	manager.RegisterResourceType(new TestResource2Loader);
	const auto filePath = s_TestFilepath / "dummyFilledSave.fileprop";
	DeleteOnTearDown(filePath);

	auto	   resource			   = std::make_shared<TestResourceWithPropertyFile>();
	const auto handleTestResource2 = manager.LoadResource<TestResource2>(testPathTest, true);
	resource->m_InnerHandle		   = handleTestResource2;
	SetFilePath(resource, filePath);
	SetDirty(resource);
	EXPECT_TRUE(resource->Save());
}

TEST_F(ResourceLoadSaveFixture, LoadDummyFilledInner_Instant)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestResourceWithPropertyFileLoader);
	const auto filePath = s_TestFilepath / "dummyFilledInstantLoad.fileprop";

	const auto handleLoaded = manager.LoadResource<TestResourceWithPropertyFile>(filePath, true);
	EXPECT_TRUE(handleLoaded.IsReady());
	const auto innerHandle = manager.GetResource<TestResource2>(testPathTest);
	EXPECT_TRUE(innerHandle.IsReady());
}

} // namespace GLEngine::Core
