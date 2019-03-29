#pragma once

#include <string>
namespace GLEngine {
namespace Core {

//=================================================================================
struct S_WindowInfo {
	S_WindowInfo(unsigned int width, unsigned int height)
		: m_width(width)
		, m_height(height) {}
	std::string m_name;
	unsigned int m_width, m_height;
	std::string m_WindowClass;

	enum class E_Driver {
		OpenGL,
		Vulkan,
		DirectX,
	};

	virtual E_Driver GetDriver() const = 0;
};

//=================================================================================
struct S_VulkanWindowInfo : public S_WindowInfo {
	virtual E_Driver GetDriver() const override {
		return S_WindowInfo::E_Driver::Vulkan;
	}
};
}}
