#include <RendererStdafx.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/Storage/TextureStorage.h>
#include <Renderer/Textures/TextureManager.h>

#include <GUI/ReflectionGUI.h>

#include <Core/Application.h>
#include <Core/Resources/ResourceManager.h>

#include <Utils/Serialization/SerializationUtils.h>

#include <imgui.h>
#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_Material>("C_Material")
		.constructor<>() // for deserialization purposes
		.constructor<std::string>()
		.property("Name", &C_Material::m_Name)
		.property("NameTitle", &C_Material::m_Name)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Text>(),
			RegisterMetamember<SerializationCls::NoSerialize>(true)
		)
		.property("Color", &C_Material::m_Color)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Colour>(),
			RegisterMetamember<UI::Colour::Name>("Colour:")
		)
		.property("Roughness", &C_Material::m_Roughness)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Slider>(),
			RegisterMetamember<UI::Slider::Name>("Roughness:"),
			RegisterMetamember<UI::Slider::Min>(0.f),
			RegisterMetamember<UI::Slider::Max>(1.0f)
		)
		.property("ColorMapRes", &C_Material::m_ColorMapRes)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Texture>(),
			RegisterMetamember<UI::Texture::Name>("Colour map"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<TextureResource>()))
		.property("NormalMapRes", &C_Material::m_NormalMapRes)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Texture>(),
			RegisterMetamember<UI::Texture::Name>("Normal map"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<TextureResource>()))
		.property("RoughnessMapRes", &C_Material::m_RoughnessRes)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Texture>(),
			RegisterMetamember<UI::Texture::Name>("Roughness map"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<TextureResource>()))
		.property("NormalMap", &C_Material::GetNormalMapPath, &C_Material::SetNormalMapPath)
		(RegisterMetamember<SerializationCls::NoSerialize>(true))
		.property("RoughnessMap", &C_Material::GetRoughnessMapPath, &C_Material::SetRoughnessMapPath)
		(RegisterMetamember<SerializationCls::NoSerialize>(true))
		.property("Shininess", &C_Material::GetShininess, &C_Material::SetShininess)
		(RegisterMetamember<SerializationCls::NoSerialize>(true));

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_Material>>();
}
// clang-format on

namespace GLEngine::Renderer {

//=================================================================================
C_Material::C_Material(const std::string& name)
	: m_Name(name)
	, m_Color(Colours::white)
	, m_Roughness(0.5f)
	, m_Changed(true)
	, m_MaterialIndex(-1)
	, m_Shininess(32.f)
{
}

//=================================================================================
C_Material::C_Material(const MeshData::Material& material)
	: m_Name(material.m_Name)
	, m_Color(material.diffuse)
	, m_Roughness(0.5f)
	, m_Changed(true)
	, m_MaterialIndex(-1)
	, m_Shininess(material.shininess)
{
}

//=================================================================================
C_Material::C_Material()
	: C_Material("")
{
}

//=================================================================================
C_Material::C_Material(C_Material&& other) noexcept
	: m_Name(std::move(other.m_Name))
	, m_Color(other.m_Color)
	, m_Roughness(other.m_Roughness)
	, m_ColorMapRes(other.m_ColorMapRes)
	, m_NormalMapRes(other.m_NormalMapRes)
	, m_RoughnessRes(other.m_RoughnessRes)
	, m_ColorMap(other.m_ColorMap)
	, m_NormalMap(other.m_NormalMap)
	, m_RoughnessMap(other.m_RoughnessMap)
	, m_Changed(other.m_Changed)
	, m_MaterialIndex(other.m_MaterialIndex)
	, m_Shininess(other.GetShininess())
{
}

//=================================================================================
C_Material C_Material::Clone() const
{
	return C_Material{""};
}

//=================================================================================
bool C_Material::IsChanged() const
{
	return m_Changed;
}

//=================================================================================
void C_Material::CleanChangeFlag()
{
	m_Changed = false;
}

//=================================================================================
void C_Material::SetDiffuseColor(const Colours::T_Colour& color)
{
	m_Color = color;
}

//=================================================================================
void C_Material::SetRoughness(const float roughness)
{
	m_Roughness = roughness;
}

//=================================================================================
void C_Material::SetNormalMap(Handle<Texture> texture)
{
	m_NormalMap = texture;
	m_Changed	= true;
}

//=================================================================================
void C_Material::SetNormalMapPath(const std::filesystem::path& path)
{
	auto& rm	   = Core::C_ResourceManager::Instance();
	m_NormalMapRes = rm.LoadResource<TextureResource>(path);
}

//=================================================================================
void C_Material::SetRoughnessMap(Handle<Texture> texture)
{
	m_Roughness	   = 1.0f;
	m_RoughnessMap = texture;
	m_Changed	   = true;
}

//=================================================================================
void C_Material::SetRoughnessMapPath(const std::filesystem::path& path)
{
	auto& rm	   = Core::C_ResourceManager::Instance();
	m_RoughnessRes = rm.LoadResource<TextureResource>(path);
}

//=================================================================================
void C_Material::SetColorMap(Handle<Texture> texture)
{
	m_ColorMap = texture;
	m_Color	   = Colours::white;
	m_Changed  = true;
}

//=================================================================================
void C_Material::SetColorMapPath(const std::filesystem::path& path)
{
	auto& rm	  = Core::C_ResourceManager::Instance();
	m_ColorMapRes = rm.LoadResource<TextureResource>(path);
}

//=================================================================================
int C_Material::GetMaterialIndex() const
{
	return m_MaterialIndex;
}

//=================================================================================
bool C_Material::DrawGUI()
{
	rttr::instance obj(*this);
	if (GUI::DrawAllPropertyGUI(obj).empty() == false)
	{
		m_Changed = true;
	}

	return m_Changed;
}

//=================================================================================
const std::filesystem::path& C_Material::GetColorMapPath() const
{
	const static std::filesystem::path empty{};
	if (m_ColorMapRes)
		return m_ColorMapRes.GetResource().GetFilePath();
	return empty;
}

//=================================================================================
const std::filesystem::path& C_Material::GetNormalMapPath() const
{
	const static std::filesystem::path empty("");
	if (m_NormalMapRes)
		return m_NormalMapRes.GetResource().GetFilePath();
	return empty;
}

//=================================================================================
const std::filesystem::path& C_Material::GetRoughnessMapPath() const
{
	const static std::filesystem::path empty("");
	if (m_RoughnessRes)
		return m_RoughnessRes.GetResource().GetFilePath();
	return empty;
}

//=================================================================================
void C_Material::Update()
{
	auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
	auto& tMGR	 = C_TextureManager::CreateInstance(device);

	const auto needsUpdateTexture = [&tMGR](Core::ResourceHandle<TextureResource> resHandle, Handle<Texture> GPUHandle) {
		auto currTexture = tMGR.GetTexture(resHandle);
		return resHandle.IsReady() && (currTexture != GPUHandle || !currTexture);
	};

	// color map is loaded but not set
	if (needsUpdateTexture(m_ColorMapRes, m_ColorMap))
	{
		Handle<Texture> colorMap = tMGR.GetOrCreateTexture(m_ColorMapRes);

		SetColorMap(colorMap);
	}
	if (m_ColorMap.IsValid() && m_ColorMapRes.IsFailed())
	{
		SetColorMap({});
	}

	// normal map is loaded but not set
	if (needsUpdateTexture(m_NormalMapRes, m_NormalMap))
	{
		Handle<Texture> normalMap = tMGR.GetOrCreateTexture(m_NormalMapRes);

		SetNormalMap(normalMap);
	}
	if (m_NormalMap.IsValid() && m_NormalMapRes.IsFailed())
	{
		SetNormalMap({});
	}

	// Roughness map is loaded but not set
	if (needsUpdateTexture(m_RoughnessRes, m_RoughnessMap))
	{
		Handle<Texture> roughnessMap = tMGR.GetOrCreateTexture(m_RoughnessRes);

		SetRoughnessMap(roughnessMap);
	}
	if (m_RoughnessMap.IsValid() && m_RoughnessRes.IsFailed())
	{
		SetRoughnessMap({});
	}
}

} // namespace GLEngine::Renderer
