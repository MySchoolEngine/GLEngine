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

#include <Renderer/IResource.h>
#include <Renderer/Textures/DeviceTexture.h>

#include <rttr/registration_friend.h>
#include <rttr/registration.h>

namespace GLEngine::Renderer {
class I_TextureViewStorage;
class C_TextureView;
} // namespace GLEngine::Renderer

namespace GLEngine::GLRenderer {
namespace Mesh {
struct Texture;
}

namespace Textures {
using T_TexBufferFuture = std::future<std::unique_ptr<Renderer::I_TextureViewStorage>>;
class C_Texture : public Renderer::I_DeviceTexture {
public:
	explicit C_Texture(const std::string& name, GLenum target = GL_TEXTURE_2D);
	C_Texture(const C_Texture&) = delete;
	C_Texture(C_Texture&& t);
	void operator=(C_Texture&& rhs);
	virtual ~C_Texture();

	// TODO move to Commands
	void bind() const;
	void unbind() const;

	inline void StartGroupOp()
	{
		bind();
		m_bGroupOperations = true;
	}
	inline void EndGroupOp()
	{
		m_bGroupOperations = false;
		unbind();
	}

	[[nodiscard]] virtual inline unsigned int	   GetWidth() const override { return m_Dimensions.x; }
	[[nodiscard]] virtual inline unsigned int	   GetHeight() const override { return m_Dimensions.y; }
	[[nodiscard]] virtual inline const glm::uvec2& GetDimensions() const override { return m_Dimensions; }
	inline void									   SetWidth(unsigned int width) { m_Dimensions.x = width; }
	inline void									   SetHeight(unsigned int height) { m_Dimensions.y = height; }
	inline void									   SetDimensions(const glm::uvec2& dim) { m_Dimensions = dim; }

	[[nodiscard]] virtual void* GetDeviceTextureHandle() const override;
	// just for now
	[[nodiscard]] inline GLuint GetTexture() const { return m_texture; }
	[[nodiscard]] inline GLenum GetTarget() const { return m_target; }

	[[nodiscard]] virtual T_TexBufferFuture GetTextureData() const override;

	[[nodiscard]] std::uint64_t CreateHandle();
	[[nodiscard]] std::uint64_t GetHandle() const;
	void						MakeHandleResident(bool val = true);

	virtual void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT) override;
	virtual void SetWrap(Renderer::E_WrapFunction wrapS, Renderer::E_WrapFunction wrapT, Renderer::E_WrapFunction wrapR) override;
	virtual void SetFilter(Renderer::E_TextureFilter min, Renderer::E_TextureFilter mag) override;
	virtual void SetBorderColor(const glm::vec4& color) override;
	virtual void GenerateMipMaps() override;
	void		 SetTexParameter(GLenum pname, const glm::vec4& value);
	void		 SetTexParameter(GLenum pname, GLint value);

	void		 SetTexData2D(int level, const Renderer::MeshData::Texture& tex);
	virtual void SetTexData2D(int level, const Renderer::I_TextureViewStorage* tex) override;
	virtual void SetTexData2D(int level, const Renderer::C_TextureView tex) override;
	void		 SetInternalFormat(Renderer::E_TextureFormat internalFormat, GLint format);

	RTTR_ENABLE(Renderer::I_DeviceTexture);
	RTTR_REGISTRATION_FRIEND;
protected:
	void Clean();

	GLuint					  m_texture;
	GLenum					  m_target;
	Renderer::E_TextureFormat m_Format;
	glm::uvec2				  m_Dimensions;
	bool					  m_bGroupOperations : 1;
	std::uint64_t			  m_Handle;
	std::string				  m_Name;
};
} // namespace Textures
} // namespace GLEngine::GLRenderer