#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/SerializationUtils.h>

#include <CoreTest/Resources/TestClasses/TestResourceWithPropertyFile.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Core;
	rttr::registration::class_<TestResourceWithPropertyFile>((TestResourceWithPropertyFile::GetResourceTypeName() + "Handle").c_str())
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("m_InnerHandle", &TestResourceWithPropertyFile::m_InnerHandle)(
			rttr::policy::prop::as_reference_wrapper,
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<DelayTestResource>()))
		;

		rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<TestResourceWithPropertyFile>>();
		rttr::type::register_converter_func([](std::shared_ptr<TestResourceWithPropertyFile> ptr, bool& ok) -> std::shared_ptr<Resource> {
			ok = true;
			return std::static_pointer_cast<Resource>(ptr);
			});

		rttr::type::register_equal_comparator<ResourceHandle<TestResourceWithPropertyFile>>();
}
// clang-format on

DECLARE_RESOURCE_HANDLE_AFTER_DESERIALIZE(TestResourceWithPropertyFile)

namespace GLEngine::Core {

std::shared_ptr<Resource> TestResourceWithPropertyFileLoader::CreateResource() const
{
	return std::make_shared<TestResourceWithPropertyFile>();
}

std::vector<std::string> TestResourceWithPropertyFileLoader::GetSupportedExtensions() const
{
	return {".fileprop"};
}
} // namespace GLEngine::Core
