#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestClasses/TestResource2.h>

DECLARE_RESOURCE_HANDLE_AFTER_DESERIALIZE(TestResource2)

namespace GLEngine::Core {

std::shared_ptr<Resource> TestResource2Loader::CreateResource() const
{
	return std::make_shared<TestResource2>();
}
std::vector<std::string> TestResource2Loader::GetSupportedExtensions() const
{
	return {".test2", ".test2-2"};
}
} // namespace GLEngine::Core
DECLARE_RESOURCE_TYPE(GLEngine::Core::TestResource2)
