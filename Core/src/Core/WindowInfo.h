#pragma once

#include <string>

namespace Core {

//=================================================================================
struct S_WindowInfo {
	S_WindowInfo(unsigned int width, unsigned int height) 
		: m_width(width)
		, m_height(height) {}
	std::string m_name;
	unsigned int m_width, m_height;

	enum class E_Driver {
		OpenGL,
		Vulkan,
		DirectX,
	};

	virtual E_Driver GetDriver() const = 0;
};

//=================================================================================
struct S_OpenGLWindowInfo : public S_WindowInfo {
	S_OpenGLWindowInfo(unsigned int width, unsigned int height)
		: S_WindowInfo(width, height) {}
	unsigned int m_MajorVersion = 4;
	unsigned int m_MinorVersion = 3;

	virtual E_Driver GetDriver() const override {
		return S_WindowInfo::E_Driver::OpenGL;
	}
};

//=================================================================================
struct S_VulkanWindowInfo : public S_WindowInfo {
	virtual E_Driver GetDriver() const override {
		return S_WindowInfo::E_Driver::Vulkan;
	}
};
}

