#pragma once

#include <Core/CoreApi.h>

#include <rttr/registration_friend.h>
#include <rttr/type>

// I want to store version, all derived resources, crc, icon, filename...
namespace GLEngine::Core {
class CORE_API_EXPORT C_Metafile {
public:
	C_Metafile() = default;
	C_Metafile(const std::filesystem::path& originalFile);

	const std::filesystem::path& GetOriginalFileName() const { return m_OriginalFilename; }

	static std::filesystem::path GetMetafileName(std::filesystem::path);

	void AddDerivedResource(const std::filesystem::path& path);

	[[nodiscard]] bool Load();
	[[nodiscard]] bool Save() const;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;

private:
	std::filesystem::path m_OriginalFilename; //< some resources can have multiple different filetypes
	std::set<std::filesystem::path> m_DerivedResources;
};
} // namespace GLEngine::Core