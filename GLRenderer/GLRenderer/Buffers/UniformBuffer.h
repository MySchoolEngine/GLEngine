/**
 * ==============================================
 * \file 		UniformBuffer.h
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

#include <GLRenderer/Buffers/GLBuffer.h>

#include <Renderer/Resources/RenderResourceHandle.h>

namespace GLEngine::Renderer {
class I_Renderer;
class ResourceManager;
}
namespace GLEngine::GLRenderer::Buffers {
class C_UniformBuffer {
public:
	using T_Base = C_GLBuffer<GL_UNIFORM_BUFFER>;
	C_UniformBuffer(const std::string& blockName, unsigned int index, Renderer::ResourceManager& resourceManager);
	virtual ~C_UniformBuffer() = default;

	[[nodiscard]] virtual std::size_t GetBufferSize() const = 0;
	virtual const void*				  Data() const			= 0;

	void UploadData(Renderer::I_Renderer& renderer) const;

	void		Activate(Renderer::ResourceManager& resourceManager, bool activate = true);
	inline bool IsActive() const { return m_active; }

	const std::string& GetBlockName() const { return m_blockName; }
	int				   GetBinding() const { return GetIndex(); }

	GLuint GetIndex() const { return m_index; }
	void   SetIndex(GLuint val) { m_index = val; }

protected:
	Renderer::Handle<Renderer::Buffer> m_Handle;

private:
	GLuint		m_index;
	std::string m_blockName;
	bool		m_active;
};
} // namespace GLEngine::GLRenderer::Buffers