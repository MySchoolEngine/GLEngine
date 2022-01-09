#pragma once

#include <Renderer/Textures/TextureDefinitions.h>

#include <rttr/registration_friend.h>
#include <rttr/registration.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;
class C_TextureView;

class I_DeviceTexture {
public:
	using T_TexBufferFuture = std::future<std::unique_ptr<I_TextureViewStorage>>;

	[[nodiscard]] virtual unsigned int		GetWidth() const			   = 0;
	[[nodiscard]] virtual unsigned int		GetHeight() const			   = 0;
	[[nodiscard]] virtual const glm::uvec2& GetDimensions() const		   = 0;
	[[nodiscard]] virtual void*				GetDeviceTextureHandle() const = 0; // todo return type
	[[nodiscard]] virtual T_TexBufferFuture GetTextureData() const		   = 0;

	virtual void SetWrap(E_WrapFunction wrapS, E_WrapFunction wrapT, E_WrapFunction wrapR) = 0;
	virtual void SetWrap(E_WrapFunction wrapS, E_WrapFunction wrapT)					   = 0;
	virtual void SetFilter(E_TextureFilter min, E_TextureFilter mag)					   = 0;
	virtual void SetBorderColor(const glm::vec4& color)									   = 0;
	virtual void GenerateMipMaps()														   = 0;

	virtual void SetTexData2D(int level, const I_TextureViewStorage* tex) = 0;
	virtual void SetTexData2D(int level, const C_TextureView tex)		  = 0;

	RTTR_ENABLE();
};
} // namespace GLEngine::Renderer
