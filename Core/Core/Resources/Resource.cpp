#include <CoreStdafx.h>

#include <Core/Resources/Resource.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Core;

	rttr::registration::class_<Resource>("Resource")
		.method("GetResourceTypeHash", &Resource::GetResourceTypeHash)
		.property("FilePath", &Resource::m_Filepath);


	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<Resource>>();
}
// clang-format on

namespace GLEngine::Core {

//=================================================================================
Resource::Resource() = default;

//=================================================================================
Resource::~Resource() = default;

//=================================================================================
bool Resource::IsReady() const
{
	return m_State == ResourceState::Ready;
}

//=================================================================================
ResourceState Resource::GetState() const
{
	return m_State;
}

//=================================================================================
const std::filesystem::path& Resource::GetFilePath() const
{
	return m_Filepath;
}

} // namespace GLEngine::Core