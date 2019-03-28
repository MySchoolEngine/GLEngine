/** ==============================================
 * @file 		ShaderStorage.h
 * @date 		2018/03/18 11:01
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <GLRenderer/Buffers/GLBuffer.h>

#include <vector>

namespace GLEngine {
namespace GLRenderer {
namespace Buffers {

class C_ShaderStorageBuffer
	: public C_GLBuffer<GL_SHADER_STORAGE_BUFFER> {
public:
	C_ShaderStorageBuffer(int bindingPoint);
	virtual ~C_ShaderStorageBuffer() = default;
	virtual void UploadData() const = 0;
	virtual void DownloadData() = 0;
	virtual void ClearBuffer() = 0;

	virtual void bind() const override;
	inline void SetBindingPoint(GLuint binding) noexcept { m_BindingPoint = binding; }
private:
	GLuint m_BindingPoint;
};
}}}