#pragma once

#include <fmt/format.h>
#include <vulkan/vulkan.h>

template <> struct fmt::formatter<VkResult> {
	constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

	template <typename FormatContext> auto format(const VkResult& result, FormatContext& ctx)
	{
		// https://www.khronos.org/registry/vulkan/specs/1.0-wsi_extensions/pdf/vkspec.pdf
		switch (result)
		{
		case VkResult::VK_SUCCESS:
			return format_to(ctx.out(), "Everything OK. (VK_SUCCESS)");
		case VkResult::VK_ERROR_OUT_OF_HOST_MEMORY:
			return format_to(ctx.out(), "A host memory allocation has failed.. (VK_ERROR_OUT_OF_HOST_MEMORY)");
		case VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY:
			return format_to(ctx.out(), "A device memory allocation has failed. (VK_ERROR_OUT_OF_DEVICE_MEMORY)");
		case VkResult::VK_ERROR_INITIALIZATION_FAILED:
			return format_to(ctx.out(), "Initialization of an object could not be completed for "
										"implementation-specific reasons.. (VK_ERROR_INITIALIZATION_FAILED)");
		case VkResult::VK_ERROR_DEVICE_LOST:
			return format_to(ctx.out(), "The logical or physical device has been lost. (VK_ERROR_DEVICE_LOST)");
		case VkResult::VK_ERROR_MEMORY_MAP_FAILED:
			return format_to(ctx.out(), "Mapping of a memory object has failed. (VK_ERROR_MEMORY_MAP_FAILED)");
		case VkResult::VK_ERROR_LAYER_NOT_PRESENT:
			return format_to(ctx.out(), "A requested layer is not present or could not be loaded. (VK_ERROR_LAYER_NOT_PRESENT)");
		case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT:
			return format_to(ctx.out(), "A requested extension is not supported. (VK_ERROR_EXTENSION_NOT_PRESENT)");
		case VkResult::VK_ERROR_FEATURE_NOT_PRESENT:
			return format_to(ctx.out(), "A requested feature is not supported. (VK_ERROR_FEATURE_NOT_PRESENT)");
		case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER:
			return format_to(ctx.out(), "The requested version of Vulkan is not supported by the driver or "
										"is otherwise incompatible for implementation-specific reasons. (VK_ERROR_INCOMPATIBLE_DRIVER)");
		case VkResult::VK_ERROR_TOO_MANY_OBJECTS:
			return format_to(ctx.out(), " Too many objects of the type have already been created. (VK_ERROR_TOO_MANY_OBJECTS)");
		case VkResult::VK_ERROR_FORMAT_NOT_SUPPORTED:
			return format_to(ctx.out(), "A requested format is not supported on this device. (VK_ERROR_FORMAT_NOT_SUPPORTED)");
		case VkResult::VK_ERROR_FRAGMENTED_POOL:
			return format_to(ctx.out(), "A pool allocation has failed due to fragmentation of the pool’s "
										"memory. This must only be returned if no attempt to allocate host or device memory was made "
										"to accommodate the new allocation. This should be returned in preference to "
										"VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation "
										"failure was due to fragmentation.(VK_ERROR_FRAGMENTED_POOL)");
		case VkResult::VK_ERROR_SURFACE_LOST_KHR:
			return format_to(ctx.out(), "A surface is no longer available. (VK_ERROR_SURFACE_LOST_KHR)");
		case VkResult::VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
			return format_to(ctx.out(), "The requested window is already in use by Vulkan or "
										"another API in a manner which prevents it from being used again. (VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)");
		case VkResult::VK_ERROR_OUT_OF_DATE_KHR:
			return format_to(ctx.out(), "A surface has changed in such a way that it is no longer compatible "
										"with the swapchain, and further presentation requests using the swapchain will fail. "
										"Applications must query the new surface properties and recreate their swapchain if they wish "
										"to continue presenting to the surface(VK_ERROR_OUT_OF_DATE_KHR)");
		case VkResult::VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			return format_to(ctx.out(), "The display used by a swapchain does not use the same "
										"presentable image layout, or is incompatible in a way that prevents sharing an image. (VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)");
		case VkResult::VK_ERROR_OUT_OF_POOL_MEMORY:
			return format_to(ctx.out(), "A pool memory allocation has failed. This must only be returned if "
										"no attempt to allocate host or device memory was made to accommodate the new allocation. If "
										"the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be "
										"returned instead. (VK_ERROR_OUT_OF_POOL_MEMORY)");
		case VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE:
			return format_to(ctx.out(), "An external handle is not a valid handle of the specified type. (VK_ERROR_INVALID_EXTERNAL_HANDLE)");
		case VkResult::VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
			return format_to(ctx.out(), "A buffer creation or memory allocation failed because "
										"the requested address is not available. A shader group handle assignment failed because the "
										"requested shader group handle information is no longer valid. (VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS)");
		case VkResult::VK_ERROR_UNKNOWN:
			return format_to(ctx.out(), "An unknown error has occurred; either the application has provided invalid "
										"input, or an implementation failure has occurred. (VK_ERROR_UNKNOWN)");
		default:
			return format_to(ctx.out(), "Unknown result");
			break;
		}
	}
};
