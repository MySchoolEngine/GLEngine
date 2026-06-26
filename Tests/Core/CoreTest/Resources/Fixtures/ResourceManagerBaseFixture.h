#pragma once

#include <Core/Resources/Metafile.h>
#include <Core/Resources/ResourceManager.h>

#include <chrono>
#include <filesystem>
#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <vector>

namespace GLEngine::Core {

// Shared base fixture for resource manager tests.
//
// Provides:
//   - SetUp/TearDown that verify the manager starts and ends empty.
//   - DeleteOnTearDown(path): registers a filesystem path to be deleted
//     during TearDown (useful for test output files written to disk).
//   - Helpers: FlushAllUnused, RemoveMetafileIfExists, VerifyManagerEmpty.
class ResourceManagerBaseFixture : public ::testing::Test {
public:
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

		for (const auto& path : m_FilesToDelete)
			RemoveFileIfExists(path);
		VerifyNoMetaFilesExist();
		VerifyManagerEmpty(manager, "TearDown");
	}

	// Register a path to be deleted during TearDown.
	void DeleteOnTearDown(const std::filesystem::path& path) { m_FilesToDelete.push_back(path); }

	template <class ResourceType> static ResourceHandle<ResourceType> CreateResourceHandle(std::shared_ptr<ResourceType> resource)
	{
		resource->m_State = ResourceState::Ready;
		return ResourceHandle<ResourceType>(resource);
	}

	static void SetFilePath(const std::shared_ptr<Resource>& resource, const std::filesystem::path& filepath) { resource->m_Filepath = filepath; }
	static void SetDirty(const std::shared_ptr<Resource>& resource) { resource->m_Dirty = true; }

	// ---------------------------------------------------------------------------
	// Helpers
	// ---------------------------------------------------------------------------

	static void FlushAllUnused(C_ResourceManager& manager)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(50));
		// s_UpdatesBeforeDelete is not enough as the outer first needs to expire, then the inner
		for (unsigned int i = 0; i <= C_ResourceManager::s_UpdatesBeforeDelete * C_ResourceManager::s_NumUpdatesBetweenUnloading * 2; ++i)
		{
			const auto numResourcesBefore = manager.m_Resources.size();
			manager.UpdatePendingLoads();
			// manager.m_UnusedList.size() is there due to resources created by hand (std::make_shared<DelayTestResource>())
			if (manager.m_Resources.size() + manager.m_UnusedList.size() == 0)
				break;
			if (numResourcesBefore != manager.m_Resources.size())
				i = 0;
		}
	}

	static void RemoveMetafileIfExists(const std::filesystem::path& resourcePath) { RemoveFileIfExists(C_Metafile::GetMetafileName(resourcePath)); }

	static void RemoveFileIfExists(const std::filesystem::path& path)
	{
		std::error_code ec;
		std::filesystem::remove(path, ec);
		std::filesystem::remove(C_Metafile::GetMetafileName(path), ec);
	}

	static void VerifyManagerEmpty(const C_ResourceManager& manager, const std::string& stage)
	{
		EXPECT_TRUE(manager.m_Resources.empty()) << stage << ": m_Resources should be empty";
		EXPECT_TRUE(manager.m_UnusedList.empty()) << stage << ": m_UnusedList should be empty";
		EXPECT_TRUE(manager.m_FinishedLoads.empty()) << stage << ": m_FinishedLoads should be empty";
		EXPECT_TRUE(manager.m_ExtToLoaders.empty()) << stage << ": m_ExtToLoaders should be empty";
		EXPECT_TRUE(manager.m_TypeIdToLoader.empty()) << stage << ": m_TypeIdToLoader should be empty";
	}

	/**
	 * @brief Verifies that no .meta files exist in the current directory.
	 */
	static void VerifyNoMetaFilesExist()
	{
		std::error_code ec;
		bool			hasMetaFiles = false;
		for (const auto& entry : std::filesystem::directory_iterator(".", ec))
		{
			EXPECT_FALSE(entry.path().extension() == ".meta") << "No .meta files should exist in working directory" << entry;
		}
	}

private:
	std::vector<std::filesystem::path> m_FilesToDelete;
};

} // namespace GLEngine::Core
