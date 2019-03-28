/**
 * ==============================================
 * \file 		GLBuffer.h
 * \date 		2018/03/17 19:06
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
namespace Buffers {

template<GLenum TYPE>
class C_GLBuffer {
public:
	C_GLBuffer();
	virtual ~C_GLBuffer();

	virtual void bind()		const;
	virtual void unbind()	const;

	constexpr GLenum GetBufferType() const noexcept;
protected:
	GLuint m_id;
};
}}}
#include <GLRenderer/Buffers/GLBuffer.inl>