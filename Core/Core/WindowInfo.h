#pragma once

#include <string>

#include <Core/CoreEnums.h>

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

	virtual E_Driver GetDriver() const = 0;
};

//=================================================================================
struct S_VulkanWindowInfo : public S_WindowInfo {
	virtual E_Driver GetDriver() const override {
		return E_Driver::Vulkan;
	}
};
}}
