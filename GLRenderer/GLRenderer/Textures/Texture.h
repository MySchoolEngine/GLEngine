/**
 * ==============================================
 * \file 		Texture.h
 * \date 		2018/03/17 19:07
 * \project 	Computer Graphics Project
 * \faculty 	Faculty of Information Technology
 * \university 	Brno University of Technology
 *
 * \author Dominik Rohacek
 * Contact: RohacekD@gmail.com
 * ==============================================
 */

#pragma once

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>
#include <GLRenderer/Helpers/TextureHelpers.h>
#include <GLRenderer/Textures/Sampler.h>

#include <Renderer/Descriptors/TextureDescriptor.h>
#include <Renderer/Textures/DeviceTexture.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;
class C_TextureView;
} // namespace GLEngine::Renderer

namespace GLEngine::GLRenderer {
class C_GLDevice;
namespace Mesh {
struct Texture;
}

namespace Textures {
using T_TexBufferFuture = std::future<std::unique_ptr<Renderer::I_TextureViewStorage>>;
class C_Texture : public Renderer::I_DeviceTexture {
public:
	explicit C_Texture(const Renderer::TextureDescriptor& desc);
	C_Texture(const C_Texture&) = delete;
	C_Texture(C_Texture&& t);
	void operator=(C_Texture&& rhs) noexcept;
	virtual ~C_Texture();

	[[nodiscard]] bool IsAllocated() const override;
	void			   SetReadyToUse() { m_IsPresentOnGPU = true; }
	bool			   IsPresentOnGPU() const { return m_IsPresentOnGPU; }

	// TODO move to Commands or remove due to new API
	void bind() const;
	void unbind() const;

	[[nodiscard]] glm::uvec2 GetDimensions() const override { return {m_Desc.width, m_Desc.height}; }
	void					 SetWidth(const unsigned int width) { m_Desc.width = width; }
	void					 SetHeight(const unsigned int height) { m_Desc.height = height; }
	void					 SetName(std::string name);
	std::string				 GetName();

	[[nodiscard]] void* GetDeviceTextureHandle() const override;
	// just for now
	void				 SetGPUID(GLuint ID) { m_Texture = ID; }
	[[nodiscard]] GLuint GetTexture() const { return m_Texture; }
	[[nodiscard]] GLuint GetDefaultSampler() const { return m_DefaultSampler.m_Sampler; }
	[[nodiscard]] GLenum GetTarget() const { return GetTextureType(m_Desc.type); }

	[[nodiscard]] T_TexBufferFuture GetTextureData() const override;

	[[nodiscard]] std::uint64_t CreateHandle();
	[[nodiscard]] std::uint64_t GetHandle() const;
	void						MakeHandleResident(bool val = true);

	void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT) override;
	void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT, Renderer::E_WrapFunction wrapR) override;
	void SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag) override;
	void SetBorderColor(const glm::vec4& color) override;
	void GenerateMipMaps() override;
	void SetParameter(GLenum pname, const glm::vec4& value);
	void SetParameter(GLenum pname, GLint value);

	void SetTexData2D(int level, const Renderer::I_TextureViewStorage* tex) override;
	void SetTexData2D(int level, const Renderer::C_TextureView tex) override;

	void* GetGPUHandle() override;

	friend class C_TextureManager;
	friend class ::GLEngine::GLRenderer::C_GLDevice;

	RTTR_ENABLE(Renderer::I_DeviceTexture);
	RTTR_REGISTRATION_FRIEND;

protected:
	void Clean();


	GLuint		  m_Texture;
	bool		  m_IsPresentOnGPU = false;
	bool		  m_IsResident	   = false;
	std::uint64_t m_Handle;

	C_Sampler2D m_DefaultSampler;
};
} // namespace Textures
} // namespace GLEngine::GLRenderer