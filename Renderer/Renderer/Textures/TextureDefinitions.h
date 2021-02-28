#pragma once

#include <Core/CoreMacros.h>

namespace GLEngine::Renderer {

//=================================================================================
enum class E_TextureChannel
{
	Red,
	Green,
	Blue,
	Alpha,
	None,
};

using T_Channels = std::array<E_TextureChannel, 4>;

//=================================================================================
enum class E_TextureFormat
{
	RGBA32f, // float
	RGB32f,
	RG32f,
	R32f,
	RGBA16f, // float
	RGB16f,
	RG16f,
	R16f,
	RGBA32i, // int
	RGB32i,
	RG32i,
	R32i,
	RGBA16i, // short
	RGB16i,
	RG16i,
	R16i,
	RGBA8i, // std::uint8_t
	RGB8i,
	RG8i,
	R8i,

	D24S8, // depth 24 stencil 8
	D32f,
	D24,
	D16,
};

//=================================================================================
inline std::uint8_t GetNumberChannels(const E_TextureFormat format)
{
	switch (format)
	{
	case E_TextureFormat::RGBA32f:
	case E_TextureFormat::RGBA16f:
	case E_TextureFormat::RGBA32i:
	case E_TextureFormat::RGBA16i:
	case E_TextureFormat::RGBA8i:
		return 4;
		break;
	case E_TextureFormat::RGB32f:
	case E_TextureFormat::RG16f:
	case E_TextureFormat::RGB32i:
	case E_TextureFormat::RGB16i:
	case E_TextureFormat::RGB8i:
		return 3;
		break;
	case E_TextureFormat::RG32f:
	case E_TextureFormat::RGB16f:
	case E_TextureFormat::RG32i:
	case E_TextureFormat::RG16i:
	case E_TextureFormat::RG8i:
		return 2;
		break;
	case E_TextureFormat::R32f:
	case E_TextureFormat::R16f:
	case E_TextureFormat::R32i:
	case E_TextureFormat::R16i:
	case E_TextureFormat::R8i:
		return 1;
		break;
	case E_TextureFormat::D24S8:
	case E_TextureFormat::D32f:
	case E_TextureFormat::D24:
	case E_TextureFormat::D16:
		return 1;
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown format.");
		return 1;
		break;
	}
}

//=================================================================================
inline std::uint8_t GetNumberChannels(const T_Channels& channels)
{
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha})
	{
		return 4;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None})
	{
		return 3;
	}
	if (channels == T_Channels{E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::Alpha})
	{
		return 4;
	}
	if (channels == T_Channels{E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::None})
	{
		return 3;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::None, E_TextureChannel::None})
	{
		return 2;
	}
	if (channels == T_Channels{E_TextureChannel::Red, E_TextureChannel::None, E_TextureChannel::None, E_TextureChannel::None})
	{
		return 1;
	}
	GLE_ERROR("Unknown channels combination.");
	return 1;
}

//=================================================================================
inline T_Channels GetChannels(const E_TextureFormat format)
{
	switch (format)
	{
	case E_TextureFormat::RGBA32f:
	case E_TextureFormat::RGBA16f:
	case E_TextureFormat::RGBA32i:
	case E_TextureFormat::RGBA16i:
	case E_TextureFormat::RGBA8i:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha};
	case E_TextureFormat::RGB32f:
	case E_TextureFormat::RG16f:
	case E_TextureFormat::RGB32i:
	case E_TextureFormat::RGB16i:
	case E_TextureFormat::RGB8i:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None};
	case E_TextureFormat::RG32f:
	case E_TextureFormat::RGB16f:
	case E_TextureFormat::RG32i:
	case E_TextureFormat::RG16i:
	case E_TextureFormat::RG8i:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::None, E_TextureChannel::None};
	case E_TextureFormat::R32f:
	case E_TextureFormat::R16f:
	case E_TextureFormat::R32i:
	case E_TextureFormat::R16i:
	case E_TextureFormat::R8i:
		return {E_TextureChannel::Red, E_TextureChannel::None, E_TextureChannel::None, E_TextureChannel::None};
	case E_TextureFormat::D24S8:
	case E_TextureFormat::D32f:
	case E_TextureFormat::D24:
	case E_TextureFormat::D16:
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown format.");
		return {};
	}
}

//=================================================================================
inline T_Channels GetOrderedChannels(const std::uint8_t numChannels)
{
	switch (numChannels)
	{
	case 4:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha};
	case 3:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None};
	case 2:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::None, E_TextureChannel::None};
	case 1:
		return {E_TextureChannel::Red, E_TextureChannel::None, E_TextureChannel::None, E_TextureChannel::None};
	default:
		GLE_ERROR("Wrong number of channels.");
		return {E_TextureChannel::Red, E_TextureChannel::None, E_TextureChannel::None, E_TextureChannel::None};
	}
}

//=================================================================================
//=================================================================================
//=================================================================================
enum class E_WrapFunction
{
	ClampToEdge,
	ClampToBorder,
	MirroredRepeat,
	Repeat,
	// MirrorClampToEdge,
};

} // namespace GLEngine::Renderer
