#pragma once

#include <Renderer/Buffer.h>
#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/DeviceTexture.h>
#include <Renderer/Textures/TextureSampler.h>

#include <memory>

/**
 * Regarding textures:
 * 1] I_DeviceTexture exists regardless of GPU Texture is allocated
 * 2] I_Device can do garbage collection once in a while (when GRAM is full) so it needs to invalidate deallocated handles.
 * 3] I can internally hold an array of all allocated memory objects with use-counts

 * Garbage collection
 * 1] Device holds an array of all allocated memory blocks as a use-counted object
 *     - use count is increased each frame no one touches the object
 *       and zeroed when someone uses the object.
 * 2] User have to ask for the handle - it is a memory handle, no data uploaded so we can do it in the same frame!
 *    (Still want to be able to return "future" handle just in case we don't have memory yet)
 * 3] When a user gives up it will be automatically deleted (can be postponed, but it is not a valid handle any more)

 * Do I really need to deal with ref-counts here?
 * I can deal with "this texture is already allocated" in the layer above - texture manager
 * This is just memory management. The update itself is being handled somewhere (too complex?)
 * This will also allow me to implement just a subset of memory management (garbage collection can come later)
 */

namespace GLEngine::Renderer {
class I_Device {
public:
	I_Device()			= default;
	virtual ~I_Device() = default;


	using T_TextureHandle = std::shared_ptr<I_DeviceTexture>;

	[[nodiscard]] virtual bool AllocateTexture(I_DeviceTexture& texture) = 0;
	virtual void			   DestroyTexture(I_DeviceTexture& texture)	 = 0;

	virtual T_TextureHandle CreateTextureHandle(const TextureDescriptor& desc) = 0; // temporary until better handles introduced

	[[nodiscard]] virtual bool		  AllocateSampler(I_TextureSampler2D& texture) = 0;
	virtual void					  DestroySampler(I_TextureSampler2D& texture)  = 0;
	[[nodiscard]] virtual std::size_t GetAllocatedMemory() const				   = 0;

	using T_BufferHandle = std::shared_ptr<I_Buffer>;

	[[nodiscard]] virtual bool AllocateBuffer(I_Buffer& buffer)					= 0;
	virtual T_BufferHandle	   CreateBufferHandle(const BufferDescriptor& desc) = 0; // temporary until better handles introduced
	virtual void			   DestroyBuffer(I_Buffer& buffer)					= 0;
};
} // namespace GLEngine::Renderer
