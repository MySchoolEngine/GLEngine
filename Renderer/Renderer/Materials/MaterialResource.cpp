#include <RendererStdafx.h>

#include <Renderer/Materials/MaterialResource.h>
#include <Renderer/Materials/PBRMaterialData.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/TextureResource.h>

#include <GUI/ReflectionGUI.h>

#include <Core/Resources/ResourceHandle.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

#include "Utils/Serialization/XMLSerialize.h"

// Specialization so ResourceHandle<MaterialResource> reloads after XML deserialization
namespace GLEngine::Core {
template <> void ResourceHandle<Renderer::MaterialResource>::AfterDeserialize(Utils::C_XMLDeserializer::DeserializeCtx& ctx)
{
	auto& rm = C_ResourceManager::Instance();
	if (GetFilePath() != "")
	{
		*this = rm.LoadResource<Renderer::MaterialResource>(GetFilePath());
	}
}
} // namespace GLEngine::Core

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Core;
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<MaterialResource>(
			(MaterialResource::GetResrourceTypeName() + "Handle").c_str())
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("MaterialName", &MaterialResource::m_MaterialName)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Text>(),
			RegisterMetamember<SerializationCls::NoSerialize>(true)
		)
		.property("ShaderPath", &MaterialResource::m_ShaderPath)
		.property("Material", &MaterialResource::m_Material)
		;

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<MaterialResource>>();
	rttr::type::register_converter_func(
		[](std::shared_ptr<MaterialResource> ptr, bool& ok) -> std::shared_ptr<Resource> {
			ok = true;
			return std::static_pointer_cast<Resource>(ptr);
		});
	rttr::type::register_equal_comparator<ResourceHandle<MaterialResource>>();
}
// clang-format on

namespace GLEngine::Renderer {

//=================================================================================
std::shared_ptr<I_MaterialData> MaterialResource::BuildPBRData(const MeshData::Material& mat, const std::vector<std::filesystem::path>& textures)
{
	auto data = std::make_shared<C_PBRMaterialData>(mat.m_Name);
	data->SetColor(mat.diffuse);

	// shininess → roughness: invert and normalize (shininess 0 = rough, 128+ = smooth)
	const float roughness = 1.f - std::clamp(mat.shininess / 128.f, 0.f, 1.f);
	data->SetRoughness(roughness);

	auto& rm = Core::C_ResourceManager::Instance();

	if (mat.textureIndex >= 0 && static_cast<std::size_t>(mat.textureIndex) < textures.size())
	{
		const auto& texPath = textures[static_cast<std::size_t>(mat.textureIndex)];
		if (!texPath.empty())
			data->SetColorMapRes(rm.LoadResource<TextureResource>(texPath));
	}

	if (mat.normalTextureIndex >= 0 && static_cast<std::size_t>(mat.normalTextureIndex) < textures.size())
	{
		const auto& normalPath = textures[static_cast<std::size_t>(mat.normalTextureIndex)];
		if (!normalPath.empty())
			data->SetNormalMapRes(rm.LoadResource<TextureResource>(normalPath));
	}

	return data;
}

//=================================================================================
bool MaterialResource::Load(const std::filesystem::path& filepath)
{
	m_Filepath = filepath;
	pugi::xml_document doc;

	pugi::xml_parse_result result = doc.load_file(m_Filepath.c_str());
	if (result.status != pugi::status_ok)
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "Can't open material file: {}", m_Filepath);
		return false;
	}

	Utils::C_XMLDeserializer d(Core::C_ResourceManager::Instance());
	auto					 loaded = d.Deserialize<std::shared_ptr<MaterialResource>>(doc);
	if (!loaded.has_value())
	{
		CORE_LOG(E_Level::Error, E_Context::Core, "XML {} is not a valid MaterialResource.", m_Filepath);
		return false;
	}

	m_MaterialName = loaded->get()->m_MaterialName;
	m_ShaderPath   = loaded->get()->m_ShaderPath;
	m_Material	   = std::move(loaded->get()->m_Material);
	return true;
}

//=================================================================================
bool MaterialResource::Reload()
{
	return Load(m_Filepath);
}

//=================================================================================
bool MaterialResource::SaveInternal() const
{
	Utils::C_XMLSerializer s;
	const auto			   doc = s.Serialize(*this);
	return doc.save_file(m_Filepath.c_str());
}

//=================================================================================
bool MaterialResource::DrawGUI()
{
	rttr::instance obj(*this);
	bool		   changed = GUI::DrawAllPropertyGUI(obj).empty() == false;

	if (m_Material)
		changed |= m_Material->DrawGUI();

	return changed;
}

//=================================================================================
std::unique_ptr<Core::I_ResourceLoader> MaterialResource::GetLoader()
{
	return std::make_unique<MaterialResourceLoader>();
}

//=================================================================================
std::shared_ptr<Core::Resource> MaterialResourceLoader::CreateResource() const
{
	return std::make_shared<MaterialResource>();
}

//=================================================================================
std::vector<std::string> MaterialResourceLoader::GetSupportedExtensions() const
{
	return {".glmat"};
}

} // namespace GLEngine::Renderer
