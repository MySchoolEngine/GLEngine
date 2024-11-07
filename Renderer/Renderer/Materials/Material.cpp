#include <RendererStdafx.h>

#include <Renderer/IDevice.h>
#include <Renderer/IRenderer.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/Textures/Storage/TextureStorage.h>

#include <GUI/ReflectionGUI.h>

#include <Core/Application.h>
#include <Core/Resources/ResourceManager.h>

#include <imgui.h>
#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_Material>("C_Material")
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
			RegisterMetamember<UI::Texture::Name>("Colour map"))
		.property("NormalMapRes", &C_Material::m_NormalMapRes)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Texture>(),
			RegisterMetamember<UI::Texture::Name>("Normal map"))
		.property("RoughnessMapRes", &C_Material::m_RoughnessRes)(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetaclass<MetaGUI::Texture>(),
			RegisterMetamember<UI::Texture::Name>("Roughness map"))
		.property("NormalMap", &C_Material::GetNormalMapPath, &C_Material::SetNormalMapPath)
		.property("RoughnessMap", &C_Material::GetRoughnessMapPath, &C_Material::SetRoughnessMapPath)
		.property("Shininess", &C_Material::GetShininess, &C_Material::SetShininess);

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_Material>>();
}
// clang-format on

namespace GLEngine::Renderer {

//=================================================================================
C_Material::C_Material(const std::string& name)
	: m_Name(name)
	, m_Color(Colours::white)
	, m_Roughness(0.5f)
	, m_ColorMap(nullptr)
	, m_NormalMap(nullptr)
	, m_RoughnessMap(nullptr)
	, m_Changed(true)
	, m_MaterialIndex(-1)
	, m_Shininess(32.f)
{
	SetColorMapPath("Models/Bricks01/REGULAR/1K/Bricks01_COL_VAR1_1K.bmp");
}

//=================================================================================
C_Material::C_Material(const MeshData::Material& material)
	: m_Name(material.m_Name)
	, m_Color(material.diffuse)
	, m_Roughness(0.5f)
	, m_ColorMap(nullptr)
	, m_NormalMap(nullptr)
	, m_RoughnessMap(nullptr)
	, m_Changed(true)
	, m_MaterialIndex(-1)
	, m_Shininess(material.shininess)
{
	SetColorMapPath("Models/Bricks01/REGULAR/1K/Bricks01_COL_VAR1_1K.bmp");
}

//=================================================================================
C_Material::C_Material(C_Material&& other)
	: m_Name(std::move(other.m_Name))
	, m_Color(other.m_Color)
	, m_Roughness(other.m_Roughness)
	, m_ColorMap(other.m_ColorMap)
	, m_NormalMap(other.m_NormalMap)
	, m_RoughnessMap(other.m_RoughnessMap)
	, m_Changed(other.m_Changed)
	, m_MaterialIndex(other.m_MaterialIndex)
	, m_Shininess(other.GetShininess())
{
	SetColorMapPath("Models/Bricks01/REGULAR/1K/Bricks01_COL_VAR1_1K.bmp");
}

//=================================================================================
C_Material C_Material::clone() const
{
	return C_Material("");
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
void C_Material::SetRoughness(float roughness)
{
	m_Roughness = roughness;
}

//=================================================================================
void C_Material::SetNormalMap(std::shared_ptr<I_DeviceTexture> texture)
{
	if (m_NormalMap)
	{
		auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
		device.DestroyTexture(*m_NormalMap.get());
	}
	m_NormalMap	  = texture;
	m_Changed = true;
}

//=================================================================================
void C_Material::SetNormalMapPath(const std::filesystem::path& path)
{
	auto& rm	   = Core::C_ResourceManager::Instance();
	m_NormalMapRes = rm.LoadResource<TextureResource>(path);
}

//=================================================================================
void C_Material::SetRoughnessMap(std::shared_ptr<I_DeviceTexture> texture)
{
	if (m_RoughnessMap)
	{
		auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
		device.DestroyTexture(*m_RoughnessMap.get());
	}
	m_Roughness	   = 1.0f;
	m_RoughnessMap = texture;
	m_Changed = true;
}

//=================================================================================
void C_Material::SetRoughnessMapPath(const std::filesystem::path& path)
{
	auto& rm	   = Core::C_ResourceManager::Instance();
	m_RoughnessRes = rm.LoadResource<TextureResource>(path);
}

//=================================================================================
void C_Material::SetColorMap(std::shared_ptr<I_DeviceTexture> texture)
{
	if (m_ColorMap)
	{
		auto& device = Core::C_Application::Get().GetActiveRenderer().GetDevice();
		device.DestroyTexture(*m_ColorMap.get());
	}
	m_ColorMap	  = texture;
	m_Color		  = Colours::white;
	m_Changed = true;
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
	// load textures?
	if (m_ColorMap == nullptr && m_ColorMapRes.IsReady())
	{
		auto&				  resource = m_ColorMapRes.GetResource();
		I_TextureViewStorage& storage  = resource.GetStorage();
		auto				  colorMap = device.CreateTextureHandle(
			 Renderer::TextureDescriptor{resource.GetFilePath().generic_string(), storage.GetDimensions().x, storage.GetDimensions().y, Renderer::E_TextureType::TEXTURE_2D,
										 Renderer::GetClosestFormat(storage.GetChannels(), !Renderer::IsIntegral(storage.GetStorageType())), false});
		if (!device.AllocateTexture(*colorMap.get())) {
			CORE_LOG(E_Level::Error, E_Context::Render, "Cannot allocate memory for texture '{}' in material '{}'", resource.GetFilePath(), GetName());
			return;
		}
		colorMap->SetTexData2D(0, (&storage));

		SetColorMap(colorMap);
	}
	if (m_ColorMap && m_ColorMapRes.IsFailed()) {
		SetColorMap(nullptr);
	}
	if (m_NormalMap == nullptr && m_NormalMapRes.IsReady())
	{
		auto&				  resource = m_NormalMapRes.GetResource();
		I_TextureViewStorage& storage  = resource.GetStorage();
		auto				  normalMap = device.CreateTextureHandle(
			 Renderer::TextureDescriptor{resource.GetFilePath().generic_string(), storage.GetDimensions().x, storage.GetDimensions().y, Renderer::E_TextureType::TEXTURE_2D,
										 Renderer::GetClosestFormat(storage.GetChannels(), !Renderer::IsIntegral(storage.GetStorageType())), false});
		if (!device.AllocateTexture(*normalMap.get()))
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Cannot allocate memory for texture '{}' in material '{}'", resource.GetFilePath(), GetName());
			return;
		}
		normalMap->SetTexData2D(0, (&storage));

		SetNormalMap(normalMap);
	}
	if (m_NormalMap && m_NormalMapRes.IsFailed())
	{
		SetNormalMap(nullptr);
	}
	if (m_RoughnessMap == nullptr && m_RoughnessRes.IsReady())
	{
		auto&				  resource	= m_RoughnessRes.GetResource();
		I_TextureViewStorage& storage	= resource.GetStorage();
		auto				  roughnessMap = device.CreateTextureHandle(
			 Renderer::TextureDescriptor{resource.GetFilePath().generic_string(), storage.GetDimensions().x, storage.GetDimensions().y, Renderer::E_TextureType::TEXTURE_2D,
										 Renderer::GetClosestFormat(storage.GetChannels(), !Renderer::IsIntegral(storage.GetStorageType())), false});
		if (!device.AllocateTexture(*roughnessMap.get()))
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Cannot allocate memory for texture '{}' in material '{}'", resource.GetFilePath(), GetName());
			return;
		}
		roughnessMap->SetTexData2D(0, (&storage));

		SetRoughnessMap(roughnessMap);
	}
	if (m_RoughnessMap && m_RoughnessRes.IsFailed())
	{
		SetRoughnessMap(nullptr);
	}
}

} // namespace GLEngine::Renderer
