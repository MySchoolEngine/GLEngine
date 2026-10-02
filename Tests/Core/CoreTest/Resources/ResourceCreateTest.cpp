#include <CoreTestStdafx.h>

#include <CoreTest/Resources/Fixtures/ResourceManagerBaseFixture.h>
#include <CoreTest/Resources/TestClasses/TestCreatableResource.h>
#include <CoreTest/Resources/TestClasses/TestResource2.h>
#include <optional>

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

TEST_F(ResourceCreateFixture, GetCreatableLoadersOnlyReturnsEmptyCreationSupportingLoaders)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResource2Loader);
	manager.RegisterResourceType(new TestCreatableResourceLoader);

	const auto creatable = manager.GetCreatableLoaders();

	ASSERT_EQ(creatable.size(), 1u);
	EXPECT_EQ(creatable[0].get().GetResourceTypeID(), TestCreatableResource::GetResourceTypeHashStatic());
}

TEST_F(ResourceCreateFixture, CreateNewResourceByLoaderRegistersResource)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResource2Loader);
	const auto loaderOpt = manager.GetLoaderForExt(".test2");
	ASSERT_TRUE(loaderOpt.has_value());

	// Wrap the returned type-erased shared_ptr in a ResourceHandleBase (same lifetime
	// management ResourceHandle<T> gives templated callers) so it's eligible for cleanup -
	// otherwise it stays tracked in m_Resources forever and TearDown's VerifyManagerEmpty fails.
	std::optional<ResourceHandleBase> handle;
	{
		const auto result = manager.CreateNewResourceByLoader(loaderOpt.value(), s_TestFilepath / testPathTest);
		ASSERT_TRUE(result.has_value());
		handle.emplace(result.value());
	}

	ASSERT_TRUE(handle.has_value());
	EXPECT_EQ(handle->GetFilePath(), (s_TestFilepath / testPathTest).lexically_normal());
}
} // namespace GLEngine::Core