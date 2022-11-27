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

#include <Renderer/Buffer.h>

#include <GLRenderer/Helpers/BufferHelpers.h>

namespace GLEngine::GLRenderer {
class C_GLDevice;
}

namespace GLEngine::GLRenderer::Buffers {

// TODO Rename once the others deleted
class C_GLBufferImpl : public Renderer::I_Buffer {
public:
	C_GLBufferImpl(const Renderer::BufferDescriptor& desc)
		: Renderer::I_Buffer(desc)
	{
	}

	// missing part!
	void bind() override { glBindBuffer(GetBufferUsage(m_Desc.type), m_id); }

	friend class ::GLEngine::GLRenderer::C_GLDevice;
protected:
	GLuint m_id;
};

class I_GLBufferBase {
public:
	virtual ~I_GLBufferBase() = default;

	virtual void bind() const	= 0;
	virtual void unbind() const = 0;

	virtual void NameBuffer(const std::string& name) const										  = 0;
	virtual void AllocateMemory(const std::size_t size, GLenum usage, const void* data = nullptr) = 0;
};

//=================================================================================
template <GLenum TYPE> class C_GLBuffer : public I_GLBufferBase {
public:
	C_GLBuffer();
	virtual ~C_GLBuffer();

	virtual void bind() const override;
	virtual void unbind() const override;

	constexpr GLenum GetBufferType() const noexcept;

	virtual void AllocateMemory(const std::size_t size, GLenum usage, const void* data = nullptr) override;

	//=================================================================================
	virtual void NameBuffer(const std::string& name) const override { glObjectLabel(GL_BUFFER, m_id, static_cast<GLsizei>(name.length()), name.c_str()); }

protected:
	GLuint m_id;
};

} // namespace GLEngine::GLRenderer::Buffers
#include <GLRenderer/Buffers/GLBuffer.inl>