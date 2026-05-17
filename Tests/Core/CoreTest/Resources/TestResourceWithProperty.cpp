#include <CoreTestStdafx.h>

#include <CoreTest/Resources/TestResourceWithProperty.h>

namespace GLEngine::Core {

std::shared_ptr<Resource> TestResourceWithPropertyLoader::CreateResource() const
{
	return std::make_shared<TestResourceWithProperty>();
}

std::vector<std::string> TestResourceWithPropertyLoader::GetSupportedExtensions() const
{
	return {".testprop"};
}

template <>
void ResourceHandle<TestResourceWithProperty>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
}

} // namespace GLEngine::Core

DECLARE_RESOURCE_TYPE(GLEngine::Core::TestResourceWithProperty)
