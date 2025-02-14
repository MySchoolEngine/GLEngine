#pragma once

#include <Core/CoreApi.h>
#include <Core/Resources/Resource.h>

#include <Utils/Serialization/XMLDeserialize.h>

#include <rttr/registration_friend.h>

namespace GLEngine::Core {
class CORE_API_EXPORT ResourceHandleBase {
public:
	ResourceHandleBase();
	explicit ResourceHandleBase(const std::shared_ptr<Resource>& resource);
	virtual ~ResourceHandleBase();
	ResourceHandleBase(ResourceHandleBase&& other) noexcept;
	ResourceHandleBase(const ResourceHandleBase& other);

	ResourceHandleBase& operator=(const ResourceHandleBase& other)
	{
		if (this == &other)
			return *this;
		m_Resource = other.m_Resource;
		return *this;
	}

	ResourceHandleBase& operator=(ResourceHandleBase&& other) noexcept
	{
		if (this == &other)
			return *this;
		m_Resource = std::move(other.m_Resource);
		return *this;
	}

	[[nodiscard]] ResourceState GetState() const;
	[[nodiscard]] bool			IsReady() const;
	[[nodiscard]] bool			IsLoading() const;
	[[nodiscard]] bool			IsFailed() const;

	explicit operator bool() const { return IsReady(); }

	const std::filesystem::path& GetFilePath() const;

	RTTR_ENABLE()

protected:
	std::shared_ptr<Resource> m_Resource; // nullptr only when no loader exists
	RTTR_REGISTRATION_FRIEND
};

// this forces include of ResourceType definition into headers, can I move it somewhere else?
template <class ResourceType> requires(is_resource<ResourceType>) class ResourceHandle final : public ResourceHandleBase {
public:
	ResourceHandle() = default;
	explicit ResourceHandle(std::shared_ptr<ResourceType> resource)
		: ResourceHandleBase(resource)
	{
	}

	const ResourceType& GetResource() const;
	ResourceType&		GetResource();

	const std::filesystem::path& GetFilepath() const;

	void AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx);

	bool operator==(const ResourceHandle other) const { return m_Resource == other.m_Resource; }

	RTTR_ENABLE(ResourceHandleBase)
};

// for usage in maps
template <is_resource ResourceType>
struct ResourceHandleCmp {
	bool operator()(const ResourceHandle<ResourceType>& lhs, const ResourceHandle<ResourceType>& rhs) const
	{
		return lhs.GetFilepath() < rhs.GetFilepath();
	}
};

} // namespace GLEngine::Core
#include <Core/Resources/ResourceHandle.inl>