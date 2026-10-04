#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

namespace GLEngine::Core {
/**
 * @brief Fake resource that supports blank creation, for testing SupportsEmptyCreation().
 */
class TestCreatableResource : public Resource {
public:
	DEFINE_RESOURCE_TYPE(TestCreatableResource)
	TestCreatableResource() = default;

	[[nodiscard]] bool Load(const std::filesystem::path& filepath, LoadCtx& ctx) override
	{
		m_Filepath = filepath;
		return true;
	}
	[[nodiscard]] bool								Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }
	bool											SupportSaving() const override { return true; }

protected:
	bool SaveInternal() const override { return true; }
};

class TestCreatableResourceLoader : public ResourceLoader<TestCreatableResource> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
	bool					  SupportsEmptyCreation() const override { return true; }
};
} // namespace GLEngine::Core
