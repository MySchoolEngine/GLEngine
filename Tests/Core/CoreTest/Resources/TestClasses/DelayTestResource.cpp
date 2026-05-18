#include <CoreTestStdafx.h>

#include <CoreTest/Resources/TestClasses/DelayTestResource.h>

namespace GLEngine::Core {
template <> void ResourceHandle<DelayTestResource>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
}

std::shared_ptr<Resource> DelayTestResourceLoader::CreateResource() const
{
	return std::make_shared<DelayTestResource>();
}

std::vector<std::string> DelayTestResourceLoader::GetSupportedExtensions() const
{
	return {".test", ".test-slow"};
}
} // namespace GLEngine::Core
DECLARE_RESOURCE_TYPE(GLEngine::Core::DelayTestResource)