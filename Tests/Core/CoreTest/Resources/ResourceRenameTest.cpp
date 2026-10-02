#include <CoreTestStdafx.h>

#include <CoreTest/Resources/Fixtures/ResourceManagerBaseFixture.h>
#include <CoreTest/Resources/TestClasses/TestCreatableResource.h>
#include <CoreTest/Resources/TestClasses/TestResourceWithPropertyFile.h>
#include <fstream>

namespace GLEngine::Core {

static const inline std::filesystem::path s_TestFilepath{"ResourceRenameFixture"};

class ResourceRenameFixture : public ResourceManagerBaseFixture {
public:
};

TEST_F(ResourceRenameFixture, RenameTrackedUnsavedResourceUpdatesBookkeepingOnly)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestCreatableResourceLoader);

	const auto oldPath = s_TestFilepath / "old.testcreate";
	const auto newPath = s_TestFilepath / "new.testcreate";
	const auto created = manager.CreateNewResource<TestCreatableResource>(oldPath);
	ASSERT_TRUE(created.has_value());

	EXPECT_TRUE(manager.RenameResource(oldPath, newPath).has_value());
	EXPECT_FALSE(std::filesystem::exists(oldPath));
	EXPECT_FALSE(std::filesystem::exists(newPath)); // never saved - nothing on disk to move

	const auto reloaded = manager.GetResource<TestCreatableResource>(newPath);
	ASSERT_TRUE(reloaded.IsReady());
	EXPECT_EQ(reloaded.GetFilePath(), newPath.lexically_normal());

	DeleteOnTearDown(oldPath);
	DeleteOnTearDown(newPath);
}

TEST_F(ResourceRenameFixture, RenameTrackedSavedResourceMovesFileOnDisk)
{
	auto& manager = C_ResourceManager::Instance();
	manager.RegisterResourceType(new TestResourceWithPropertyFileLoader);
	std::filesystem::create_directories(s_TestFilepath);

	const auto oldPath = s_TestFilepath / "old.fileprop";
	const auto newPath = s_TestFilepath / "new.fileprop";
	const auto created = manager.CreateNewResource<TestResourceWithPropertyFile>(oldPath);
	ASSERT_TRUE(created.has_value());
	ASSERT_TRUE(created.value().GetResource().Save());
	ASSERT_TRUE(std::filesystem::exists(oldPath));

	EXPECT_TRUE(manager.RenameResource(oldPath, newPath).has_value());
	EXPECT_FALSE(std::filesystem::exists(oldPath));
	EXPECT_TRUE(std::filesystem::exists(newPath));

	const auto reloaded = manager.GetResource<TestResourceWithPropertyFile>(newPath);
	ASSERT_TRUE(reloaded.IsReady());
	EXPECT_EQ(reloaded.GetFilePath(), newPath.lexically_normal());

	DeleteOnTearDown(newPath);
}

TEST_F(ResourceRenameFixture, RenameUntrackedPlainFileJustMovesIt)
{
	std::filesystem::create_directories(s_TestFilepath);
	const auto oldPath = s_TestFilepath / "plain_old.txt";
	const auto newPath = s_TestFilepath / "plain_new.txt";
	std::ofstream(oldPath).close();

	auto& manager = C_ResourceManager::Instance();
	EXPECT_TRUE(manager.RenameResource(oldPath, newPath).has_value());
	EXPECT_FALSE(std::filesystem::exists(oldPath));
	EXPECT_TRUE(std::filesystem::exists(newPath));

	DeleteOnTearDown(newPath);
}

TEST_F(ResourceRenameFixture, RenameFailsIfDestinationAlreadyExists)
{
	std::filesystem::create_directories(s_TestFilepath);
	const auto oldPath = s_TestFilepath / "src.txt";
	const auto newPath = s_TestFilepath / "dst.txt";
	std::ofstream(oldPath).close();
	std::ofstream(newPath).close();

	auto&	   manager = C_ResourceManager::Instance();
	const auto result  = manager.RenameResource(oldPath, newPath);
	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), RenameError::DestinationExists);

	DeleteOnTearDown(oldPath);
	DeleteOnTearDown(newPath);
}

TEST_F(ResourceRenameFixture, RenameFailsIfSourceDoesNotExist)
{
	auto&	   manager = C_ResourceManager::Instance();
	const auto result  = manager.RenameResource(s_TestFilepath / "missing.txt", s_TestFilepath / "new.txt");
	ASSERT_FALSE(result.has_value());
	EXPECT_EQ(result.error(), RenameError::SourceNotFound);
}
} // namespace GLEngine::Core
