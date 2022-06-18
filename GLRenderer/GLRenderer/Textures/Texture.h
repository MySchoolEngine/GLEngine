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
	void operator=(C_Texture&& rhs);
	virtual ~C_Texture();

	[[nodiscard]] virtual bool IsAllocated() const override;
	void					   SetReadyToUse() { m_IsPresentOnGPU = true; }

	// TODO move to Commands or remove due to new API
	void bind() const;
	void unbind() const;

	[[nodiscard]] virtual inline glm::uvec2 GetDimensions() const override { return {m_Desc.width, m_Desc.height}; }
	inline void								SetWidth(unsigned int width) { m_Desc.width = width; }
	inline void								SetHeight(unsigned int height) { m_Desc.height = height; }
	inline void								SetDimensions(const glm::uvec2& dim)
	{
		// todo: should go away
		m_Desc.width  = dim.x;
		m_Desc.height = dim.y;
	}

	[[nodiscard]] virtual void* GetDeviceTextureHandle() const override;
	// just for now
	void SetGPUID(GLuint ID)
	{
		m_texture = ID;
	}
	[[nodiscard]] inline GLuint GetTexture() const { return m_texture; }
	[[nodiscard]] inline GLuint GetDefaultSampler() const { return m_DefaultSampler.m_Sampler; }
	[[nodiscard]] inline GLenum GetTarget() const { return GetTextureType(m_Desc.type); }

	[[nodiscard]] virtual T_TexBufferFuture GetTextureData() const override;

	[[nodiscard]] std::uint64_t CreateHandle();
	[[nodiscard]] std::uint64_t GetHandle() const;
	void						MakeHandleResident(bool val = true);

	virtual void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT) override;
	virtual void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT, Renderer::E_WrapFunction wrapR) override;
	virtual void SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag) override;
	virtual void SetBorderColor(const glm::vec4& color) override;
	virtual void GenerateMipMaps() override;
	void		 SetParameter(GLenum pname, const glm::vec4& value);
	void		 SetParameter(GLenum pname, GLint value);

	virtual void SetTexData2D(int level, const Renderer::I_TextureViewStorage* tex) override;
	virtual void SetTexData2D(int level, const Renderer::C_TextureView tex) override;

	virtual void* GetGPUHandle() override;

	friend class C_TextureManager;
	friend class GLEngine::GLRenderer::C_GLDevice;

protected:
	void Clean();


	GLuint					  m_texture;
	bool					  m_IsPresentOnGPU = false;
	bool					  m_IsResidient = false;
	std::uint64_t			  m_Handle;

	C_Sampler2D m_DefaultSampler;
};
} // namespace Textures
} // namespace GLEngine::GLRenderer