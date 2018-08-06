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

#include <QOpenGLFunctions_4_4_Core>

#include <glm/glm.hpp>
#include <string>

namespace GLW {
	//wrapper for opengl texture
	// for now I only going to work with GL_TEXTURE_2D
	class C_Texture 
		: public QOpenGLFunctions_4_4_Core
	{
	public:
		C_Texture(GLenum target = GL_TEXTURE_2D);
		C_Texture(const std::string& name, GLenum target = GL_TEXTURE_2D);
		virtual ~C_Texture();

		void bind();
		void unbind();

		inline void StartGroupOp() { bind(); m_bGroupOperations = true;  }
		inline void EndGroupOp() { m_bGroupOperations = false; unbind(); }

		inline unsigned int GetWidth() const { return m_Dimensions.x; }
		inline unsigned int GetHeight() const { return m_Dimensions.y; }
		inline void SetWidth(unsigned int width) { m_Dimensions.x = width; }
		inline void SetHeight(unsigned int height) { m_Dimensions.y = height; }
		inline const glm::uvec2& GetDimensions() const { return m_Dimensions; }
		inline void SetDimensions(const glm::uvec2& dim) { m_Dimensions = dim; }

		//just for now
		inline GLuint GetTexture() const { return m_texture; }
		inline GLenum GetTarget() const { return m_target; }

		void SetWrap(GLint wrapS, GLint wrapT);
		void SetFilter(GLint min, GLint mag);
		void SetTexParameter(GLenum pname, const glm::vec4& value);
		void SetTexParameter(GLenum pname, GLint value);

	protected:
		GLuint m_texture;
		GLenum m_target;
		glm::uvec2 m_Dimensions;
		bool m_bGroupOperations : 1;
	};
};