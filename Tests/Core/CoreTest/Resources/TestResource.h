#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

#include <chrono>
#include <thread>

namespace GLEngine::Core {
class TestResource : public Resource {
public:
	DEFINE_RESOURCE_TYPE(TestResource)
	TestResource()
		: testData(42)
		, testName("DefaultTest")
	{
	}

	[[nodiscard]] bool Load(const std::filesystem::path& filepath) override
	{
		m_Filepath = filepath;
		// Simulate slow loading
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		return true;
	}
	[[nodiscard]] bool								Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }

	int			testData = 0;
	std::string testName;
};

class TestResourceLoader : public ResourceLoader<TestResource> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
};
} // namespace GLEngine::Core