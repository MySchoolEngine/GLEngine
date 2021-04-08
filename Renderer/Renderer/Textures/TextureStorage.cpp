#include <RendererStdafx.h>

#include <Renderer/Textures/TextureStorage.h>

namespace GLEngine::Renderer {

//=================================================================================
// I_TextureViewStorage
//=================================================================================
I_TextureViewStorage::I_TextureViewStorage(std::size_t width, std::size_t height)
	: m_Dimensions({width, height})
	, m_Channels({E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha})
{
}

//=================================================================================
I_TextureViewStorage::~I_TextureViewStorage() = default;

//=================================================================================
void I_TextureViewStorage::Set(double value, std::size_t position)
{
	if (position >= static_cast<std::size_t>(m_Dimensions.x) * m_Dimensions.y * GetNumElements())
	{
		CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
		return;
	}
	SetInternal(value, position);
}

//=================================================================================
void I_TextureViewStorage::Set(int value, std::size_t position)
{
	if (position >= static_cast<std::size_t>(m_Dimensions.x) * m_Dimensions.y * GetNumElements())
	{
		CORE_LOG(E_Level::Info, E_Context::Render, "Writing outside of texture buffer. Result would be discarded.");
		return;
	}
	SetInternal(value, position);
}

//=================================================================================
glm::ivec2 I_TextureViewStorage::GetDimensions() const
{
	return m_Dimensions;
}

//=================================================================================
bool I_TextureViewStorage::IsSwizzled() const
{
	return !(m_Channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha}
			 || m_Channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None}
			 || m_Channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::None, E_TextureChannel::None}
			 || m_Channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::None, E_TextureChannel::None, E_TextureChannel::None});
}

} // namespace GLEngine::Renderer