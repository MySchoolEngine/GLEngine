#pragma once

#include <Core/CoreMacros.h>

#include <Utils/BitField.h>
#include <Utils/Logging/LoggingMacros.h>

#include <array>

namespace GLEngine::Renderer {

//=================================================================================
// MinMag filters
enum class E_TextureFilter : std::uint8_t
{
	Linear,
	Nearest,
	NearestMipMapNearest,
	LinearMipMapNearest,
	LinearMipMapLinear,
	NearestMipMapLinear,
};

//=================================================================================
enum class E_TextureChannel : std::uint8_t
{
	Red	  = BIT(0),
	Green = BIT(1),
	Blue  = BIT(2),
	Alpha = BIT(3),
	None  = BIT(4),
};

using T_Channels	 = std::array<E_TextureChannel, 4>;		  // When order of channels is needed
using T_ChannelsBits = ::Utils::C_BitField<E_TextureChannel>; // When order independent
} // namespace GLEngine::Renderer
// Enable bit field for channels for ease of use
template <> struct Utils::enable_BitField_operators<GLEngine::Renderer::E_TextureChannel> {
	static constexpr bool enable = true;
};
namespace GLEngine::Renderer {

//=================================================================================
// Blending
//=================================================================================
enum class E_BlendFactor : std::uint8_t
{
	Zero,
	One,
	SourceColor,
	InvSourceColor,
	SourceAlpha,
	InvSourceAlpha,
	DestinationColor,
	InvDestinationColor,
	DestinationAlpha,
	InvDestinationAlpha,
	AlphaConstant,	  //< glBlendColor, ID3D12GraphicsCommandList::OMSetBlendFactor
	InvAlphaConstant,  //< glBlendColor, ID3D12GraphicsCommandList::OMSetBlendFactor
	ColorConstant,	  //< glBlendColor, ID3D12GraphicsCommandList::OMSetBlendFactor
	InvColorConstant, //< glBlendColor, ID3D12GraphicsCommandList::OMSetBlendFactor
};

//=================================================================================
// Source is colour written by fragment shader
enum class E_BlendFunction : std::uint8_t
{
	Add,			 //< Result = Src + Dst
	Subtract,		 //< Result = Src - Dst
	ReverseSubtract, //< Result = Dst - Src
	Min,			 //< Result = Min(Dst, Src)
	Max,			 //< Result = Max(Dst, Src)
};

//=================================================================================
enum class E_TextureFormat : std::uint8_t
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
	RGBA8isrgb, // std::uint8_t srgb
	BGRA8isrgb,
	RGB8isrgb,
	RG8isrgb,
	R8isrgb,

	D24S8, // depth 24 stencil 8
	D32f,
	D24,
	D16,

	Count
};

//=================================================================================
enum class E_TextureTypes : std::uint8_t
{
	IntegralNormalized, //[ 0,1]
	Integral,
	SignedNormalized, //[-1,1]
	Signed,
	Floating,
};

[[nodiscard]] constexpr bool IsIntegral(const E_TextureTypes e)
{
	if (e == E_TextureTypes::Floating)
		return false;
	return true;
}

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
	case E_TextureFormat::RGBA8isrgb:
	case E_TextureFormat::BGRA8isrgb:
		return 4;
	case E_TextureFormat::RGB32f:
	case E_TextureFormat::RG16f:
	case E_TextureFormat::RGB32i:
	case E_TextureFormat::RGB16i:
	case E_TextureFormat::RGB8i:
	case E_TextureFormat::RGB8isrgb:
		return 3;
	case E_TextureFormat::RG32f:
	case E_TextureFormat::RGB16f:
	case E_TextureFormat::RG32i:
	case E_TextureFormat::RG16i:
	case E_TextureFormat::RG8i:
	case E_TextureFormat::RG8isrgb:
		return 2;
	case E_TextureFormat::R32f:
	case E_TextureFormat::R16f:
	case E_TextureFormat::R32i:
	case E_TextureFormat::R16i:
	case E_TextureFormat::R8i:
	case E_TextureFormat::R8isrgb:
		return 1;
	case E_TextureFormat::D24S8:
	case E_TextureFormat::D32f:
	case E_TextureFormat::D24:
	case E_TextureFormat::D16:
		return 1;
	default:
		CORE_LOG(E_Level::Error, E_Context::Render, "Unknown format.");
		return 1;
	}
}

//=================================================================================
inline constexpr bool IsDepthFormat(const Renderer::E_TextureFormat format)
{
	switch (format)
	{
	case E_TextureFormat::RGBA32f:
	case E_TextureFormat::RGBA16f:
	case E_TextureFormat::RGBA32i:
	case E_TextureFormat::RGBA16i:
	case E_TextureFormat::RGBA8i:
	case E_TextureFormat::RGB32f:
	case E_TextureFormat::RG16f:
	case E_TextureFormat::RGB32i:
	case E_TextureFormat::RGB16i:
	case E_TextureFormat::RGB8i:
	case E_TextureFormat::RG32f:
	case E_TextureFormat::RGB16f:
	case E_TextureFormat::RG32i:
	case E_TextureFormat::RG16i:
	case E_TextureFormat::RG8i:
	case E_TextureFormat::R32f:
	case E_TextureFormat::R16f:
	case E_TextureFormat::R32i:
	case E_TextureFormat::R16i:
	case E_TextureFormat::R8i:
	case E_TextureFormat::RGBA8isrgb:
	case E_TextureFormat::BGRA8isrgb:
	case E_TextureFormat::RGB8isrgb:
	case E_TextureFormat::RG8isrgb:
	case E_TextureFormat::R8isrgb:
		return false;
	case E_TextureFormat::D24S8:
	case E_TextureFormat::D32f:
	case E_TextureFormat::D24:
	case E_TextureFormat::D16:
		return true;
	default:
		GLE_ERROR("Unknown format.");
		return false;
		// keep it this way to force every new format to define this.
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
	case E_TextureFormat::RGBA8isrgb:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::Alpha};
	case E_TextureFormat::BGRA8isrgb:
		return {E_TextureChannel::Blue, E_TextureChannel::Green, E_TextureChannel::Red, E_TextureChannel::Alpha};
	case E_TextureFormat::RGB32f:
	case E_TextureFormat::RG16f:
	case E_TextureFormat::RGB32i:
	case E_TextureFormat::RGB16i:
	case E_TextureFormat::RGB8i:
	case E_TextureFormat::RGB8isrgb:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::Blue, E_TextureChannel::None};
	case E_TextureFormat::RG32f:
	case E_TextureFormat::RGB16f:
	case E_TextureFormat::RG32i:
	case E_TextureFormat::RG16i:
	case E_TextureFormat::RG8i:
	case E_TextureFormat::RG8isrgb:
		return {E_TextureChannel::Red, E_TextureChannel::Green, E_TextureChannel::None, E_TextureChannel::None};
	case E_TextureFormat::R32f:
	case E_TextureFormat::R16f:
	case E_TextureFormat::R32i:
	case E_TextureFormat::R16i:
	case E_TextureFormat::R8i:
	case E_TextureFormat::R8isrgb:
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
inline E_TextureFormat GetClosestFormat(const T_Channels& channels, const bool isFloatingPoint)
{
	GLE_ASSERT(isFloatingPoint, "Engine doesn't support integer textures yet.");
	const auto numChannels = GetNumberChannels(channels);
	switch (numChannels)
	{
	case 1:
		return E_TextureFormat::R32f;
	case 2:
		return E_TextureFormat::RG32f;
	case 3:
		return E_TextureFormat::RGB32f;
	case 4:
		return E_TextureFormat::RGBA32f;
	default:
		break;
	}
	return E_TextureFormat::RGBA32f;
}

//=================================================================================
//=================================================================================
//=================================================================================
enum class E_WrapFunction : std::uint8_t
{
	ClampToEdge,
	ClampToBorder,
	MirroredRepeat,
	Repeat,
	// MirrorClampToEdge,
};

} // namespace GLEngine::Renderer
