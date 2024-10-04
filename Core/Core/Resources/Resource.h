#pragma once

#include <Core/CoreApi.h>
#include <Core/EventSystem/EventReciever.h>

namespace GLEngine::Core {

#define DECLARE_RESOURCE_TYPE(resourceType)                                                                                                                                        \
public:                                                                                                                                                                            \
	static std::size_t GetResourceTypeHash()                                                                                                                                       \
	{                                                                                                                                                                              \
		static std::size_t hash = std::hash<std::string>{}(#resourceType);                                                                                                         \
		return hash;                                                                                                                                                               \
	}                                                                                                                                                                              \
	static std::string& GetResrourceTypeName()                                                                                                                                     \
	{                                                                                                                                                                              \
		static std::string name(#resourceType);                                                                                                                                    \
		return name;                                                                                                                                                               \
	}


enum class ResourceState : std::uint8_t
{
	Empty,
	Loading,
	Ready,
	Failed, // cannot load/no loader
};

class Resource;

template <typename T> concept IsBeDerivedResource = requires(T t)
{
	std::derived_from<T, Resource>;
	std::derived_from<typename T::T_BaseResource, Resource>;
	{
		t.IsDerived()
	}
	->std::convertible_to<bool>; // defined as static function
};

template <class T, class TBaseRes> concept IsBuildableResource = requires(T t, TBaseRes b)
{
	{
		t.Build(b)
	}
	->std::convertible_to<bool>;
};

template <typename T> concept BuildableResource = requires(T t)
{
	IsBeDerivedResource<T> && IsBuildableResource<T, typename T::T_BaseResource>;
};

// derived resources should take base resource as only constructor argument
class CORE_API_EXPORT Resource : public I_EventReciever {
public:
	Resource();
	virtual ~Resource();

	[[nodiscard]] virtual bool Load(const std::filesystem::path& filepath) = 0;
	[[nodiscard]] virtual bool Reload()									   = 0;
	[[nodiscard]] bool		   Save() const
	{
		if (!m_Dirty)
			return true;
		if (SupportSaving())
			return SaveInternal();
		return false;
	}

	[[nodiscard]] ResourceState GetState() const;
	[[nodiscard]] bool			IsReady() const;

	virtual bool SupportSaving() const { return false; }
	void		 OnEvent(I_Event& event) override {}

	const std::filesystem::path& GetFilePath() const;

protected:
	virtual bool SaveInternal() const { return false; }

	bool				  m_Dirty = false;
	std::filesystem::path m_Filepath;

private:
	ResourceState m_State = ResourceState::Empty;
	friend class C_ResourceManager;
};

template <class ResourceType> concept is_resource = std::is_base_of_v<Resource, ResourceType>;
} // namespace GLEngine::Core
