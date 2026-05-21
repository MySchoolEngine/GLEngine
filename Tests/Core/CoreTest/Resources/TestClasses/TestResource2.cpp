#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceManager.h>

#include <CoreTest/Resources/TestClasses/TestResource2.h>

namespace GLEngine::Core {
template <> void ResourceHandle<TestResource2>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
	if (GetFilePath() != "")
	{
		*this = ctx.m_ResMng.LoadResource<TestResource2>(GetFilePath(), ctx.bLoadHandlesInstantly);
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
