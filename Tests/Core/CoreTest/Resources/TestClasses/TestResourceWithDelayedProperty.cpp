#include <CoreTestStdafx.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <pugixml.hpp>

#include <CoreTest/Resources/TestClasses/TestResourceWithDelayedProperty.h>


DECLARE_RESOURCE_HANDLE_AFTER_DESERIALIZE(GLEngine::Core::TestResourceWithDelayedProperty)

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Core;

	rttr::registration::class_<ResourceHandle<TestResourceWithDelayedProperty>>((TestResourceWithDelayedProperty::GetResourceTypeName() + "Handle").c_str())
		.constructor<>()(rttr::policy::ctor::as_object)
		.method("AfterDeserialize", &ResourceHandle<TestResourceWithDelayedProperty>::AfterDeserialize)();

	rttr::registration::class_<TestResourceWithDelayedProperty>("TestResourceWithDelayedProperty")
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.method("GetResourceTypeHash", &TestResourceWithDelayedProperty::GetResourceTypeHash)
		.method("GetResourceTypeHashStatic", &TestResourceWithDelayedProperty::GetResourceTypeHashStatic)
		.property("InnerHandle", &TestResourceWithDelayedProperty::m_InnerHandle)
		;

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<TestResourceWithDelayedProperty>>();
	rttr::type::register_converter_func(
		[](std::shared_ptr<TestResourceWithDelayedProperty> ptr, bool& ok) -> std::shared_ptr<Resource> {
			ok = true;
			return std::static_pointer_cast<Resource>(ptr);
		});
	rttr::type::register_equal_comparator<ResourceHandle<TestResourceWithDelayedProperty>>();
}
// clang-format on

namespace GLEngine::Core {

bool TestResourceWithDelayedProperty::Load(const std::filesystem::path& filepath, LoadCtx& ctx)
{
	m_Filepath = filepath;
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(m_Filepath.c_str());
	if (result.status != pugi::status_ok)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open material file: {}", m_Filepath);
		return false;
	}

	Utils::C_XMLDeserializer d(ctx.m_ResMng, ctx.m_Query, ctx.m_isBlocking);
	auto					 loaded = d.Deserialize<std::shared_ptr<TestResourceWithDelayedProperty>>(doc);
	if (!loaded.has_value())
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "XML {} is not a valid TestResourceWithDelayedProperty.", m_Filepath);
		return false;
	}

	m_InnerHandle = loaded->get()->m_InnerHandle;
	ctx.m_Query.AddHandle(m_InnerHandle);
	return true;
}
std::shared_ptr<Resource> TestResourceWithDelayedPropertyLoader::CreateResource() const
{
	return std::make_shared<TestResourceWithDelayedProperty>();
}

std::vector<std::string> TestResourceWithDelayedPropertyLoader::GetSupportedExtensions() const
{
	return {".testpropdelay"};
}

} // namespace GLEngine::Core
