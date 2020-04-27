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

#include <Renderer/IResource.h>

#include <GLRenderer/Helpers/OpenGLTypesHelpers.h>

namespace GLEngine {
namespace GLRenderer {
namespace Mesh {
struct Texture;
}

namespace Textures {
class C_Texture// : public Renderer::I_Resource
{
public:
	C_Texture(const std::string& name, GLenum target = GL_TEXTURE_2D);
	C_Texture(const C_Texture&) = delete;
	C_Texture(C_Texture&& t);
	void operator=(C_Texture&& rhs);
	virtual ~C_Texture();

	void bind() const;
	void unbind() const;

	inline void StartGroupOp() { bind(); m_bGroupOperations = true; }
	inline void EndGroupOp() { m_bGroupOperations = false; unbind(); }

	[[nodiscard]] inline unsigned int GetWidth() const { return m_Dimensions.x; }
	[[nodiscard]] inline unsigned int GetHeight() const { return m_Dimensions.y; }
	inline void SetWidth(unsigned int width) { m_Dimensions.x = width; }
	inline void SetHeight(unsigned int height) { m_Dimensions.y = height; }
	[[nodiscard]] inline const glm::uvec2& GetDimensions() const { return m_Dimensions; }
	inline void SetDimensions(const glm::uvec2& dim) { m_Dimensions = dim; }

	//just for now
	[[nodiscard]] inline GLuint GetTexture() const { return m_texture; }
	[[nodiscard]] inline GLenum GetTarget() const { return m_target; }

	void SetWrap(E_WrapFunction wrapS, E_WrapFunction wrapT);
	void SetWrap(E_WrapFunction wrapS, E_WrapFunction wrapT, E_WrapFunction wrapR);
	void SetFilter(GLint min, GLint mag);
	void SetTexParameter(GLenum pname, const glm::vec4& value);
	void SetTexParameter(GLenum pname, GLint value);
	void GenerateMipMaps();

	void SetTexData2D(int level, const Renderer::MeshData::Texture& tex);
	void SetInternalFormat(GLint internalFormat, GLint format, GLenum type);
protected:
	void Clean();

	GLuint			m_texture;
	GLenum			m_target;
	glm::uvec2	m_Dimensions;
	bool				m_bGroupOperations : 1;
};
}}}