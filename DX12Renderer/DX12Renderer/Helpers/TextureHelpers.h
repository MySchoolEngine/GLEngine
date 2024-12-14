#pragma once

#include <Renderer/Textures/TextureDefinitions.h>

#include <Core/CoreMacros.h>

#include <d3d12.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
[[nodiscard]] constexpr D3D12_BLEND_OP GetBlendFunction(Renderer::E_BlendFunction e)
{
	switch (e)
	{
	case Renderer::E_BlendFunction::Add:
		return D3D12_BLEND_OP_ADD;
	case Renderer::E_BlendFunction::Subtract:
		return D3D12_BLEND_OP_SUBTRACT;
	case Renderer::E_BlendFunction::ReverseSubtract:
		return D3D12_BLEND_OP_REV_SUBTRACT;
	case Renderer::E_BlendFunction::Min:
		return D3D12_BLEND_OP_MIN;
	case Renderer::E_BlendFunction::Max:
		return D3D12_BLEND_OP_MAX;
	default:
		GLE_ASSERT(false, "Unknown blend function");
		return D3D12_BLEND_OP_ADD;\
	}
}

//=================================================================================
[[nodiscard]] constexpr D3D12_BLEND GetBlendFactor(Renderer::E_BlendFactor e)
{
	switch (e)
	{
	case Renderer::E_BlendFactor::Zero:
		return D3D12_BLEND_ZERO;
	case Renderer::E_BlendFactor::One:
		return D3D12_BLEND_ONE;
	case Renderer::E_BlendFactor::SourceColor:
		return D3D12_BLEND_SRC_COLOR;
	case Renderer::E_BlendFactor::InvSourceColor:
		return D3D12_BLEND_INV_SRC_COLOR;
	case Renderer::E_BlendFactor::SourceAlpha:
		return D3D12_BLEND_SRC_ALPHA;
	case Renderer::E_BlendFactor::InvSourceAlpha:
		return D3D12_BLEND_INV_SRC_ALPHA;
	case Renderer::E_BlendFactor::DestinationColor:
		return D3D12_BLEND_DEST_COLOR;
	case Renderer::E_BlendFactor::InvDestinationColor:
		return D3D12_BLEND_INV_DEST_COLOR;
	case Renderer::E_BlendFactor::DestinationAlpha:
		return D3D12_BLEND_DEST_ALPHA;
	case Renderer::E_BlendFactor::InvDestinationAlpha:
		return D3D12_BLEND_INV_DEST_ALPHA;
	case Renderer::E_BlendFactor::AlphaConstant:
		return D3D12_BLEND_BLEND_FACTOR;
	case Renderer::E_BlendFactor::InvAlphaConstant:
		return D3D12_BLEND_INV_BLEND_FACTOR;
	case Renderer::E_BlendFactor::ColorConstant:
		return D3D12_BLEND_BLEND_FACTOR;
	case Renderer::E_BlendFactor::InvColorConstant:
		return D3D12_BLEND_INV_BLEND_FACTOR;
	default:
		GLE_ASSERT(false, "Unknown blend factor");
		return D3D12_BLEND_ONE;
	}
}

} // namespace GLEngine::DX12Renderer