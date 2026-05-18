#include <CoreTestStdafx.h>

#include <CoreTest/Resources/TestClasses/DelayFileTestResource.h>

namespace GLEngine::Core {
template <> void ResourceHandle<DelayFileTestResource>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
}

std::shared_ptr<Resource> TestResourceLoader::CreateResource() const
{
	return std::make_shared<DelayFileTestResource>();
}

std::vector<std::string> TestResourceLoader::GetSupportedExtensions() const
{
	return {".test", ".test-slow"};
}
} // namespace GLEngine::Core
DECLARE_RESOURCE_TYPE(GLEngine::Core::DelayFileTestResource)