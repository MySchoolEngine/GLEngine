#pragma once

#include <Core/WindowInfo.h>


namespace GLEngine::DX12Renderer {

//=================================================================================
struct S_D3D12WindowInfo : public Core::S_WindowInfo {
	S_D3D12WindowInfo(unsigned int width, unsigned int height)
		: S_WindowInfo(width, height) {}

	HINSTANCE m_HInstance;

	virtual E_Driver GetDriver() const override {
		return S_WindowInfo::E_Driver::DirectX12;
	}
};

}