#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

namespace GLEngine::Core {
class TestResource2 : public Resource {
public:
	DEFINE_RESOURCE_TYPE(TestResource2)
	TestResource2()
		: value(100)
		, description("DefaultDescription")
	{
	}

	[[nodiscard]] bool Load(const std::filesystem::path& filepath) override
	{
		m_Filepath = filepath;
		return true;
	}
	[[nodiscard]] bool								Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader> GetLoader() override { return nullptr; }

	int			value = 0;
	std::string description;
};


class TestResource2Loader : public ResourceLoader<TestResource2> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
};
} // namespace GLEngine::Core
