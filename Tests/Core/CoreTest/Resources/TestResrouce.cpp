#include <CoreTestStdafx.h>

#include <CoreTest/Resources/TestResource.h>

namespace GLEngine::Core {
template <> void ResourceHandle<TestResource>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
}
} // namespace GLEngine::Core
DECLARE_RESOURCE_TYPE(GLEngine::Core::TestResource)