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


namespace GLEngine {
namespace GLRenderer {

namespace Textures {
class C_Texture;
}

namespace Components {
class C_Framebuffer {
public:
	C_Framebuffer();
	~C_Framebuffer() = default;

	void Bind();
	void Unbind();

	void AttachTexture(GLenum attachement, std::shared_ptr<Textures::C_Texture> texture);
	std::shared_ptr<Textures::C_Texture> GetAttachement(GLenum attachement);

private:
	GLuint m_FBO;
	std::map<GLenum, std::shared_ptr<Textures::C_Texture>> m_attachements;
};
}}}