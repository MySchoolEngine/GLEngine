#pragma once

#include <Core/WindowInfo.h>

#include <Core/CoreEnums.h>

namespace GLEngine::DX12Renderer {

//=================================================================================
struct S_D3D12WindowInfo : public Core::S_WindowInfo {
	S_D3D12WindowInfo(unsigned int width, unsigned int height)
		: S_WindowInfo(width, height)
		, m_HInstance(nullptr) {}

	HINSTANCE m_HInstance;

	virtual Core::E_Driver GetDriver() const override {
		return Core::E_Driver::DirectX12;
	}
};

}