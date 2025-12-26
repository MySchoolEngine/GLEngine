#include <CoreStdafx.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Reflection/Metadata.h>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Core;
	using namespace Utils::Reflection;

	rttr::registration::class_<ResourceHandleBase>("ResourceHandleBase")
		.constructor<std::shared_ptr<Resource>>()()
		.constructor<>()()
		.property("Resource", &ResourceHandleBase::m_Resource);
}
// clang-format on

namespace GLEngine::Core {

//=================================================================================
ResourceHandleBase::ResourceHandleBase(const std::shared_ptr<Resource>& resource)
	: m_Resource(resource)
{
}

//=================================================================================
ResourceHandleBase::ResourceHandleBase()
	: m_Resource(nullptr)
{
}

//=================================================================================
ResourceState ResourceHandleBase::GetState() const
{
	if (m_Resource)
		return m_Resource->GetState();
	return ResourceState::Failed;
}

//=================================================================================
ResourceHandleBase::~ResourceHandleBase()
{
	if (m_Resource.use_count() == 2)
	{
		C_ResourceManager::Instance().AddResourceToUnusedList(m_Resource);
	}
}

//=================================================================================
ResourceHandleBase::ResourceHandleBase(ResourceHandleBase&& other) noexcept = default;

//=================================================================================
ResourceHandleBase::ResourceHandleBase(const ResourceHandleBase& other) = default;

//=================================================================================
bool ResourceHandleBase::IsReady() const
{
	return GetState() == ResourceState::Ready;
}

//=================================================================================
bool ResourceHandleBase::IsLoading() const
{
	return GetState() == ResourceState::Loading;
}

//=================================================================================
bool ResourceHandleBase::IsFailed() const
{
	return GetState() == ResourceState::Failed;
}

//=================================================================================
const std::filesystem::path& ResourceHandleBase::GetFilePath() const
{
	if (m_Resource)
		return m_Resource->GetFilePath();
	return R"()";
}
} // namespace GLEngine::Core