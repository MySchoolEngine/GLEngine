#pragma once

#include <Core/CoreEnums.h>
#include <Core/WindowInfo.h>

namespace GLEngine::VkRenderer {

//=================================================================================
struct S_VkWindowInfo : public Core::S_WindowInfo {
	S_VkWindowInfo(unsigned int width, unsigned int height)
		: S_WindowInfo(width, height)
		, m_Instance(nullptr)
	{
	}

	virtual Core::E_Driver GetDriver() const override { return Core::E_Driver::Vulkan; }

	VkInstance_T* m_Instance;
};

} // namespace GLEngine::VkRenderer