#include <CoreTestStdafx.h>

#include <CoreTest/Resources/TestResourceBuildable.h>

namespace GLEngine::Core {

bool TestResourceBuildable::Load(const std::filesystem::path& filepath)
{
	m_Filepath = filepath;
	// Buildable resources don't load directly, they are built
	return false;
}

bool TestResourceBuildable::Build(const TestResource& baseResource)
{
	// Build from base resource
	builtData = baseResource.testData * 2;
	builtName = "Built_" + baseResource.testName;
	return true;
}

bool TestResourceBuildable::SaveInternal() const
{
	// Simple save implementation for testing
	return true;
}

std::shared_ptr<Resource> TestResourceBuildableLoader::CreateResource() const
{
	return std::make_shared<TestResourceBuildable>();
}

std::vector<std::string> TestResourceBuildableLoader::GetSupportedExtensions() const
{
	return {".testbuild"};
}

template <>
void ResourceHandle<TestResourceBuildable>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
}

} // namespace GLEngine::Core

DECLARE_RESOURCE_TYPE(GLEngine::Core::TestResourceBuildable)
