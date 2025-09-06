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

#include <Renderer/Descriptors/BufferDescriptor.h>

namespace GLEngine::GLRenderer {
class GLBuffer final {
public:
	explicit GLBuffer(const Renderer::BufferDescriptor& desc)
		: desc(desc)
		, m_ID(0)
		, m_MappedMemory(nullptr)
	{
	}

	const Renderer::BufferDescriptor& GetDesc() const { return desc; }

	GLenum GetType() const;
	GLenum GetUsage() const;
	uint32_t GetSize() const;

	void   bind() const;
	void   unbind() const;
	/**
	 * Binds buffer to indexed array of bound buffers
	 * https://registry.khronos.org/OpenGL-Refpages/gl4/html/glBindBufferBase.xhtml
	 * @param index index in the array of buffers
	 */
	void   BindBase(unsigned int index);

private:
	const Renderer::BufferDescriptor desc;
	GLuint							 m_ID;
	void*							 m_MappedMemory;

	friend class GLResourceManager;
};

} // namespace GLEngine::GLRenderer

namespace GLEngine::GLRenderer::Buffers {

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
	~C_GLBuffer() override;

	void bind() const override;
	void unbind() const override;

	constexpr GLenum GetBufferType() const noexcept;

	void AllocateMemory(const std::size_t size, GLenum usage, const void* data = nullptr) override;

	//=================================================================================
	void NameBuffer(const std::string& name) const override { glObjectLabel(GL_BUFFER, m_ID, static_cast<GLsizei>(name.length()), name.c_str()); }

protected:
	GLuint m_ID;
};

} // namespace GLEngine::GLRenderer::Buffers
#include <GLRenderer/Buffers/GLBuffer.inl>