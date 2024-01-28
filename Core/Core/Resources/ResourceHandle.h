#pragma once

#include <Core/CoreApi.h>
#include <Core/Resources/Resource.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Core {
class CORE_API_EXPORT ResourceHandleBase {
public:
	ResourceHandleBase();
	ResourceHandleBase(std::shared_ptr<Resource> resource);
	virtual ~ResourceHandleBase();
	[[nodiscard]] ResourceState GetState() const;
	[[nodiscard]] bool			IsReady() const;
	[[nodiscard]] bool			IsLoading() const;
	[[nodiscard]] bool			IsFailed() const;

	explicit operator bool() const { return IsReady(); }

	const std::filesystem::path& GetFilePath() const;

	RTTR_ENABLE();

protected:
	std::shared_ptr<Resource> m_Resource; // nullptr only when no loader exists
	RTTR_REGISTRATION_FRIEND;
};

// this forces include of ResourceType definition into headers, can I move it somewhere else?
template <class ResourceType> requires(is_resource<ResourceType>) class ResourceHandle : public ResourceHandleBase {
public:
	ResourceHandle() = default;
	ResourceHandle(std::shared_ptr<ResourceType> resource)
		: ResourceHandleBase(resource)
	{
	}

	const ResourceType& GetResource() const;
	ResourceType&		GetResource();

	void AfterDeserialize(GLEngine::Utils::C_XMLDeserializer::DeserializeCtx& ctx);

	RTTR_ENABLE(ResourceHandleBase);
};

} // namespace GLEngine::Core
#include <Core/Resources/ResourceHandle.inl>