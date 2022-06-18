#pragma once

#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/Textures/TextureDefinitions.h>

#include <glm/glm.hpp>

#include <future>

// Textures uses lazy load. This means they are constructed with @TextureDescriptor
// and lives as empty shells. The textures are allocated only through @I_Device
// and their size cannot change during its lifetime. Instead they should be
// deleted and allocated again. Thanks to @I_Device it should be simpler
// to implement other APIs and control memory usage.
//
// Load is being dealt with far away from the end user.
// When user issues draw call with given texture, renderer will check whether
// texture is already loaded. If not use "missing texture" instead. (Stage 1)
// There is also a problem with handles.
// User can also issue "precache" flag. (stage 2)

namespace GLEngine::Renderer {
class I_TextureViewStorage;
class C_TextureView;

class I_DeviceTexture {
public:
	using T_TexBufferFuture = std::future<std::unique_ptr<I_TextureViewStorage>>;

	I_DeviceTexture(const TextureDescriptor& desc)
		: m_Desc(desc)
	{}
	I_DeviceTexture(I_DeviceTexture&&)			  = default;
	I_DeviceTexture& operator=(I_DeviceTexture&&) = default;
	virtual ~I_DeviceTexture()					  = default;

	//< correct implementation would involve GPU calls. Should be implemented on I_Device
	I_DeviceTexture(const I_DeviceTexture&) = default;
	I_DeviceTexture& operator=(const I_DeviceTexture&) = delete;

	[[nodiscard]] virtual bool IsAllocated() const = 0;

	[[nodiscard]] unsigned int				GetWidth() const { return GetDimensions().x; };
	[[nodiscard]] unsigned int				GetHeight() const { return GetDimensions().y; };
	[[nodiscard]] virtual glm::uvec2		GetDimensions() const		   = 0;
	[[nodiscard]] virtual void*				GetDeviceTextureHandle() const = 0;
	[[nodiscard]] virtual T_TexBufferFuture GetTextureData() const		   = 0;

	virtual void SetWrap(E_WrapFunction wrapS, E_WrapFunction wrapT, E_WrapFunction wrapR) = 0;
	virtual void SetWrap(E_WrapFunction wrapS, E_WrapFunction wrapT)					   = 0;
	virtual void SetFilter(E_TextureFilter min, E_TextureFilter mag)					   = 0;
	virtual void SetBorderColor(const glm::vec4& color)									   = 0;
	virtual void GenerateMipMaps()														   = 0;

	virtual void SetTexData2D(int level, const I_TextureViewStorage* tex) = 0;
	virtual void SetTexData2D(int level, const C_TextureView tex)		  = 0;

	const TextureDescriptor& GetDescriptor() const { return m_Desc; }

	// utility function for GUI
	virtual void* GetGPUHandle() = 0;

protected:
	TextureDescriptor m_Desc;
};
} // namespace GLEngine::Renderer
