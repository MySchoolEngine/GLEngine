#pragma once

#include <Core/Resources/Resource.h>
#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceLoader.h>

#include <CoreTest/Resources/TestResource.h>

namespace GLEngine::Core {
class TestResourceBuildable : public Resource {
public:
	DEFINE_RESOURCE_TYPE(TestResourceBuildable)
	using T_BaseResource = TestResource;

	TestResourceBuildable()
		: builtData(0)
		, builtName("NotBuilt")
	{
	}

	[[nodiscard]] static constexpr bool					  IsDerived() { return true; }
	[[nodiscard]] bool									  Load(const std::filesystem::path& filepath) override;
	[[nodiscard]] bool									  Reload() override { return false; }
	[[nodiscard]] std::unique_ptr<I_ResourceLoader>		  GetLoader() override { return nullptr; }

	[[nodiscard]] bool Build(const TestResource& baseResource);

	bool SupportSaving() const override { return true; }

protected:
	bool SaveInternal() const override;

public:
	int			builtData;
	std::string builtName;
};

class TestResourceBuildableLoader : public ResourceLoader<TestResourceBuildable> {
public:
	std::shared_ptr<Resource> CreateResource() const override;
	std::vector<std::string>  GetSupportedExtensions() const override;
};

static_assert(IsBeDerivedResource<TestResourceBuildable>, "Must be derived resource");
static_assert(IsBuildableResource<TestResourceBuildable, TestResourceBuildable::T_BaseResource>, "Must be buildable");
} // namespace GLEngine::Core
