#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <CoreTest/Resources/TestClasses/TestCreatableResource.h>

DECLARE_RESOURCE_HANDLE_AFTER_DESERIALIZE(TestCreatableResource)

namespace GLEngine::Core {

std::shared_ptr<Resource> TestCreatableResourceLoader::CreateResource() const
{
	return std::make_shared<TestCreatableResource>();
}
std::vector<std::string> TestCreatableResourceLoader::GetSupportedExtensions() const
{
	return {".testcreate"};
}
} // namespace GLEngine::Core
DECLARE_RESOURCE_TYPE(GLEngine::Core::TestCreatableResource)
