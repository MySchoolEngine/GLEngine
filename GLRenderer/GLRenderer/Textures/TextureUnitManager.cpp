#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/Commands/Textures/GLActivateTexture.h>
#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>
#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <Renderer/IRenderer.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer::Textures {

//=================================================================================
C_TextureUnitManger::C_TextureUnitManger() = default;

//=================================================================================
C_TextureUnitManger& C_TextureUnitManger::Instance()
{
	static C_TextureUnitManger instance; // Guaranteed to be destroyed.
										 // Instantiated on first use.
	return instance;
}


//=================================================================================
void C_TextureUnitManger::BindTextureToUnit(const C_Texture& texture, unsigned int unit)
{
	if (m_TextureUnits[unit] == texture.GetTexture())
	{
		return;
	}

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::C_GLActivateTexture>(unit));

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&texture, unit]() {
			glBindTexture(texture.GetTarget(), texture.GetTexture()); /* here really goes the bind*/
			glBindSampler(unit, texture.GetDefaultSampler());
	  },
		fmt::format("BindTextureToUnit - {} -> {}", texture.GetTexture(), unit)));
	m_TextureUnits[unit] = texture.GetTexture();
}

//=================================================================================
bool C_TextureUnitManger::UnitHasTexture(unsigned int unit) const
{
	return m_TextureUnits.find(unit) != m_TextureUnits.end();
}

//=================================================================================
bool C_TextureUnitManger::IsTextureBindInUnit(const C_Texture& texture, unsigned int unit) const
{
	const auto it = m_TextureUnits.find(unit);
	if (it == m_TextureUnits.end())
	{
		return false;
	}

	if (it->second == texture.GetTexture())
	{
		return true;
	}
	return false;
}

//=================================================================================
bool C_TextureUnitManger::IsTextureBindInAny(const C_Texture& texture) const
{
	return GetTextureUnit(texture) >= 0;
}

//=================================================================================
unsigned int C_TextureUnitManger::GetTextureUnit(const C_Texture& texture) const
{
	auto it = std::find_if(m_TextureUnits.begin(), m_TextureUnits.end(), [&](const auto& it) { return it.second == texture.GetTexture(); });
	return it->first;
}

//=================================================================================
// Images
//=================================================================================
void C_TextureUnitManger::BindImageToUnit(const C_Texture& image, unsigned int unit, E_OpenGLAccess access)
{
	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::C_GLActivateTexture>(unit));

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&image, unit, access]() { glBindImageTexture(unit, image.GetTexture(), 0, GL_TRUE, 0, AccesRightsToEnum(access), GL_R32F); },
		fmt::format("BindImageToUnit - {} -> {}", image.GetTexture(), unit)));

	m_ImageUnits[unit] = image.GetTexture();
}

//=================================================================================
bool C_TextureUnitManger::UnitHasImage(unsigned int unit) const
{
	return m_ImageUnits.find(unit) != m_ImageUnits.end();
}

//=================================================================================
bool C_TextureUnitManger::IsImageBindInUnit(const C_Texture& image, unsigned int unit) const
{
	const auto it = m_ImageUnits.find(unit);
	if (it == m_ImageUnits.end())
	{
		return false;
	}

	if (it->second == image.GetTexture())
	{
		return true;
	}
	return false;
}

//=================================================================================
bool C_TextureUnitManger::IsImageBindInAny(const C_Texture& image) const
{
	return GetImageUnit(image) >= 0;
}

//=================================================================================
unsigned int C_TextureUnitManger::GetImageUnit(const C_Texture& image) const
{
	auto it = std::find_if(m_ImageUnits.begin(), m_ImageUnits.end(), [&](const auto& it) { return it.second == image.GetTexture(); });
	return it->first;
}

//=================================================================================
void C_TextureUnitManger::Reset()
{
	m_TextureUnits.clear();
	m_ImageUnits.clear();
}

} // namespace GLEngine::GLRenderer::Textures