#include <CoreStdafx.h>

#include <Core/Resources/Metafile.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/XMLDeserialize.h>
#include <Utils/Serialization/XMLSerialize.h>

#include <rttr/registration>

RTTR_REGISTRATION
{
	using namespace GLEngine::Core;
	rttr::registration::class_<C_Metafile>("metadata")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("OriginalResource", &C_Metafile::m_OriginalFilename)
		.property("DerivedResources", &C_Metafile::m_DerivedResources);
}

namespace GLEngine::Core {

//=================================================================================
C_Metafile::C_Metafile(const std::filesystem::path& originalFile)
	: m_OriginalFilename(originalFile)
{
}

//=================================================================================
bool C_Metafile::Load()
{
	pugi::xml_document doc;

	const auto			   metafileName = GetMetafileName(m_OriginalFilename);
	if (const pugi::xml_parse_result result = doc.load_file(metafileName.generic_string().c_str()); result.status != pugi::status_ok)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open config meta file for name: {}", metafileName);
		return false;
	}

	Utils::C_XMLDeserializer d(Core::C_ResourceManager::Instance());
	auto					 newThis = d.Deserialize<C_Metafile>(doc);
	if (newThis.has_value() == false)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "XML {} is not valid metafile file.", metafileName);
		return false;
	}
	*this = newThis.value();
	return true;
}

//=================================================================================
bool C_Metafile::Save() const
{
	Utils::C_XMLSerializer s;
	const auto			   str			= s.Serialize(*this);
	const auto			   metafileName = GetMetafileName(m_OriginalFilename);
	str.save_file(metafileName.generic_string().c_str());
	return true;
}

//=================================================================================
std::filesystem::path C_Metafile::GetMetafileName(std::filesystem::path path)
{
	return path.replace_extension("meta");
}

//=================================================================================
void C_Metafile::AddDerivedResource(const std::filesystem::path& path)
{
	m_DerivedResources.insert(path);
}

} // namespace GLEngine::Core
