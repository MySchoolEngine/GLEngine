/**
 * ==============================================
 * \file 		Framebuffer.h
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

#include <QOpenGLFunctions_4_3_Core>

#include <map>
#include <memory>


namespace GLW {
	class C_Texture;
	class C_Framebuffer : public QOpenGLFunctions_4_3_Core {
	public:
		C_Framebuffer();
		~C_Framebuffer();

		void Bind();
		void Unbind();

		void AttachTexture(GLenum attachement, std::shared_ptr<C_Texture> texture);
		std::shared_ptr<C_Texture> GetAttachement(GLenum attachement);

	private:
		GLuint m_FBO;
		std::map<GLenum, std::shared_ptr<C_Texture>> m_attachements;
	};
}