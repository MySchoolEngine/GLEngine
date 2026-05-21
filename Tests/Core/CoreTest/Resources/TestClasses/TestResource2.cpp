#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestClasses/TestResource2.h>

namespace GLEngine::Core {
template <> void ResourceHandle<TestResource2>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
	auto& rm = C_ResourceManager::Instance();
	if (GetFilePath() != "")
	{
		*this = rm.LoadResource<TestResource2>(GetFilePath());
	}
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
