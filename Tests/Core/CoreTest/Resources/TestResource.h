#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

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
		return false;
	}
	[[nodiscard]] bool								Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }

	int			testData = 0;
	std::string testName;
};
} // namespace GLEngine::Core