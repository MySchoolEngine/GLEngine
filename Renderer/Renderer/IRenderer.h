#pragma once

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/TextureManager.h>

namespace GLEngine::Renderer {

class I_Device;
class ResourceManager;
class I_TextureViewStorage;

enum class E_PassType
{
	ShadowPass,
	FinalPass
};

// We need some separated tasks -> game engine should give us list of all
// visible objects or some way to ask him what is inside given frustum than we
// need to be able to collect data to internal structures than convert them to
// GPU-friendly data than collect draw calls, render batches etc. sort them to
// some convenient order, and commit them to GPU in this way we can split one
// frame to separated parts 1] simulation 2] data collection (simulation have to
// be stopped in this moment, so everything we would be able to do here is just
// collection in tight loop) 3] data conversion - now we can begin with
// simulation of new frame 4] commit to GPU 5] check the time, if we have enough
// time, render thread can help with simulation, game computation etc. 6] render
// new frame
class I_Renderer {
public:
	using T_CommandPtr = std::unique_ptr<I_RenderCommand>;
	using T_BatchPtr   = std::unique_ptr<I_RenderBatch>;

	virtual ~I_Renderer()						  = default;
	virtual void Lock(bool lock = true)			  = 0;
	virtual void AddCommand(T_CommandPtr)		  = 0;
	virtual void AddTransferCommand(T_CommandPtr) = 0;

	/** ==============================================
	 * @method:    Commit
	 * @return:    void
	 * @brief	   Commits all GPU calls to GPU
	 ** ==============================================*/
	virtual void Commit() const = 0;

	/** ==============================================
	 * @method:    ClearCommandBuffers
	 * @return:    void
	 * @brief	   Should be called at the end of frame.
	 ** ==============================================*/
	virtual void ClearCommandBuffers() = 0;

	/** ==============================================
	 * @method:    SetBufferData
	 * @return:    void
	 * @brief	   Copy data into GPU buffer.
	 ** ==============================================*/
	virtual void SetBufferData(Handle<Buffer> dstBuffer, std::size_t numBytes, const void* data) = 0;
	virtual void SetTextureData(Handle<Texture> dstTexture, const I_TextureViewStorage& storage) {}

	virtual void SetTextureSampler(Handle<Texture> dstTexture, Handle<Sampler> srcSampler) = 0;

	virtual I_Device&		GetDevice() = 0;
	virtual ResourceManager& GetRM()		= 0;

	virtual void* GetTextureGUIHandle(Handle<Texture> texture) { return nullptr; }
	virtual C_TextureManager& GetTextureManager() { return C_TextureManager::CreateInstance(GetDevice()); }
	// another one for destroy needed

	virtual E_PassType GetCurrentPassType() const = 0;
	virtual void	   SetCurrentPassType(E_PassType type) {}
};

} // namespace GLEngine::Renderer