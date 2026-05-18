#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceLoader.h>

#include <chrono>
#include <thread>

namespace GLEngine::Core {

/**
 * @brief Main purpose of this test resource is to leave delay in loading.
 * This allows us to test async/sync loading orders.
 *
 * Features:
 *  - Delay
 *  - Loads from XML
 */
class DelayFileTestResource : public Resource {
public:
	DEFINE_RESOURCE_TYPE(DelayFileTestResource)
	DelayFileTestResource()
		: testData(42)
		, testName("DefaultTest")
	{
	}

	[[nodiscard]] bool Load(const std::filesystem::path& filepath, LoadCtx& ctx) override
	{
		m_Filepath = filepath;
		// Simulate slow loading
		std::this_thread::sleep_for(s_LoadTime);
		return true;
	}
	[[nodiscard]] bool								Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }

	int			testData = 0;
	std::string testName;

	constexpr static std::chrono::milliseconds s_LoadTime = std::chrono::milliseconds(100);
};

class TestResourceLoader : public ResourceLoader<DelayFileTestResource> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
};
} // namespace GLEngine::Core