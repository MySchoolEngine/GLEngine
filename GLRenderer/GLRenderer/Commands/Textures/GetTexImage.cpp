#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/Textures/GetTexImage.h>

#include <Renderer/Textures/Storage/TextureLinearStorage.h>

namespace GLEngine::GLRenderer::Commands {

//=================================================================================
C_GetTexImage::C_GetTexImage(std::promise<std::unique_ptr<Renderer::I_TextureViewStorage>>&& prom,
							 GLenum															 target,
							 int															 level,
							 GLenum															 format,
							 GLenum															 type,
							 std::size_t													 width,
							 std::size_t													 height,
							 std::uint8_t													 channels)
	: m_Promise(std::move(prom))
	, m_Target(target)
	, m_Level(level)
	, m_Format(format)
	, m_Type(type)
	, m_Width(width)
	, m_Height(height)
	, m_Channels(channels)
{
}

//=================================================================================
void C_GetTexImage::Commit()
{
	// TODO also needs update to the glGetTextureImage
	auto buffer = std::make_unique<Renderer::C_TextureViewStorageCPU<std::uint8_t>>(m_Width, m_Height, m_Channels);
	glGetTexImage(m_Target, m_Level, m_Format, m_Type, buffer->GetData());
	m_Promise.set_value(std::move(buffer));
}

//=================================================================================
Renderer::I_RenderCommand::E_Type C_GetTexImage::GetType() const
{
	return Renderer::I_RenderCommand::E_Type::StateChange;
}

//=================================================================================
std::string C_GetTexImage::GetDescriptor() const
{
	return fmt::format("GetTexImage target '{}' level '{}' format '{}' type '{}'", m_Target, m_Level, m_Format, m_Type);
}

} // namespace GLEngine::GLRenderer::Commands
