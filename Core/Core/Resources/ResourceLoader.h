#pragma once

#include <Core/CoreApi.h>
#include <Core/Resources/Resource.h>

namespace GLEngine::Core {
// one resource could have multiple extensions. This means no exact mapping to resource IDs
// this forces me, to group different extensions under one loader
class CORE_API_EXPORT I_ResourceLoader {
public:
	I_ResourceLoader()													   = default;
	I_ResourceLoader(const I_ResourceLoader& other)						   = default;
	I_ResourceLoader(I_ResourceLoader&& other) noexcept					   = default;
	I_ResourceLoader& operator=(const I_ResourceLoader& other)			   = default;
	I_ResourceLoader& operator=(I_ResourceLoader&& other) noexcept		   = default;
	virtual ~I_ResourceLoader()											   = default;
	[[nodiscard]] virtual std::shared_ptr<Resource> CreateResource() const = 0;
	bool											LoadResource(const std::filesystem::path& filepath, std::shared_ptr<Resource>& resource) const;
	/**
	 * List of supported extensions accompanied by the dot.
	 */
	virtual std::vector<std::string> GetSupportedExtensions() const = 0;
	virtual std::size_t				 GetResourceTypeID() const		= 0;
};

// To create loader, please inherit from this loader and register with manager
template <class ResourceType> requires(is_resource<ResourceType>) class ResourceLoader : public I_ResourceLoader {
public:
	~ResourceLoader() override = default;
	std::size_t GetResourceTypeID() const final { return ResourceType::GetResourceTypeHashStatic(); }
};

} // namespace GLEngine::Core