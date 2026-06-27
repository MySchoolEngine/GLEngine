#include <CoreTestStdafx.h>

#include <CoreTest/Resources/Fixtures/ResourceManagerBaseFixture.h>
#include <CoreTest/Resources/TestClasses/TestResource2.h>

namespace GLEngine::Core {


static const inline std::filesystem::path s_TestFilepath{"ResourceCreateFixture"};
static inline const std::filesystem::path testPathTest{"test_resource.test2"};

// ---------------------------------------------------------------------------
// Fixture
// ---------------------------------------------------------------------------
class ResourceCreateFixture : public ResourceManagerBaseFixture {
public:
};

TEST_F(ResourceCreateFixture, CreateFileNotExists)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResource2Loader);
	const auto newResource = manager.CreateNewResource<TestResource2>(s_TestFilepath / testPathTest);

	EXPECT_TRUE(newResource.has_value());
	EXPECT_TRUE(newResource.value().IsReady());
}

TEST_F(ResourceCreateFixture, CreateFileExists)
{
	std::filesystem::create_directory(s_TestFilepath);
	std::ofstream output(s_TestFilepath / testPathTest);
	output.close();
	DeleteOnTearDown(s_TestFilepath / testPathTest);

	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResource2Loader);
	const auto newResource = manager.CreateNewResource<TestResource2>(s_TestFilepath / testPathTest);

	EXPECT_FALSE(newResource);
}

TEST_F(ResourceCreateFixture, CreateFileAlreadyCreated)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResource2Loader);
	const auto newResource = manager.CreateNewResource<TestResource2>(s_TestFilepath / testPathTest);

	EXPECT_TRUE(newResource.has_value());
	EXPECT_TRUE(newResource.value().IsReady());

	const auto newResource2 = manager.CreateNewResource<TestResource2>(s_TestFilepath / testPathTest);
	EXPECT_FALSE(newResource2);
}
}