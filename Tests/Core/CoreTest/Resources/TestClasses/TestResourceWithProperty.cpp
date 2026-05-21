#include <CoreTestStdafx.h>

#include <CoreTest/Resources/TestClasses/TestResourceWithProperty.h>

namespace GLEngine::Core {

std::shared_ptr<Resource> TestResourceWithPropertyLoader::CreateResource() const
{
	return std::make_shared<TestResourceWithProperty>();
}

std::vector<std::string> TestResourceWithPropertyLoader::GetSupportedExtensions() const
{
	return {".testprop"};
}

} // namespace GLEngine::Core

DECLARE_RESOURCE_HANDLE_AFTER_DESERIALIZE(TestResourceWithProperty)
DECLARE_RESOURCE_TYPE(GLEngine::Core::TestResourceWithProperty)
