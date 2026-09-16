#include <CoreTestStdafx.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/SerializationUtils.h>

#include <CoreTest/Resources/TestClasses/TestResourceWithHandleContainers.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Core;
	rttr::registration::class_<TestResourceWithHandleContainers>((TestResourceWithHandleContainers::GetResourceTypeName() + "Handle").c_str())
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("m_HandleVector", &TestResourceWithHandleContainers::m_HandleVector)
		.property("m_HandleMap", &TestResourceWithHandleContainers::m_HandleMap)
		;

		rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<TestResourceWithHandleContainers>>();
		rttr::type::register_converter_func([](std::shared_ptr<TestResourceWithHandleContainers> ptr, bool& ok) -> std::shared_ptr<Resource> {
			ok = true;
			return std::static_pointer_cast<Resource>(ptr);
			});

		rttr::type::register_equal_comparator<ResourceHandle<TestResourceWithHandleContainers>>();
}
// clang-format on

DECLARE_RESOURCE_HANDLE_AFTER_DESERIALIZE(TestResourceWithHandleContainers)

namespace GLEngine::Core {

std::shared_ptr<Resource> TestResourceWithHandleContainersLoader::CreateResource() const
{
	return std::make_shared<TestResourceWithHandleContainers>();
}

std::vector<std::string> TestResourceWithHandleContainersLoader::GetSupportedExtensions() const
{
	return {".handlecontainers"};
}
} // namespace GLEngine::Core
