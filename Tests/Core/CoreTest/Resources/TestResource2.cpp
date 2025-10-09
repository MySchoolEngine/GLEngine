#include <CoreTestStdafx.h>

#include <CoreTest/Resources/TestResource2.h>

namespace GLEngine::Core {
template <> void ResourceHandle<TestResource2>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
}
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
