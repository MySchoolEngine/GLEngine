#pragma once

#include <Core/CoreApi.h>
#include <Core/Resources/Resource.h>

namespace GLEngine::Core {
// one resource could have multiple extensions. This means no exact mapping to resource IDs
// this forces me, to group different extensions under one loader
class CORE_API_EXPORT I_ResourceLoader {
public:
	virtual ~I_ResourceLoader()											   = default;
	virtual [[nodiscard]] std::shared_ptr<Resource> CreateResource() const = 0;
	bool											LoadResource(const std::filesystem::path& filepath, std::shared_ptr<Resource>& resource) const;
	/**
	 * List of supported extensions accompanied with the dot.
	 */
	virtual std::vector<std::string> GetSupportedExtensions() const = 0;
	virtual std::size_t				 GetResourceTypeID() const		= 0;
};

// To create loader, please inherit from this loader and register with manager
template <class ResourceType> 
requires(is_resource<ResourceType>)
class ResourceLoader : public I_ResourceLoader {
public:
	virtual std::size_t GetResourceTypeID() const final { return ResourceType::GetResourceTypeHash(); }
};

} // namespace GLEngine::Core