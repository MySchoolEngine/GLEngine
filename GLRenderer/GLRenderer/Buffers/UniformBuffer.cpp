#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UniformBuffer.h>
#include <GLRenderer/GLResourceManager.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Resources/ResourceManager.h>


namespace GLEngine::GLRenderer::Buffers {

//=================================================================================
C_UniformBuffer::C_UniformBuffer(const std::string& blockName, unsigned int index, Renderer::ResourceManager& resourceManager)
	: m_index(static_cast<GLuint>(index))
	, m_blockName(blockName)
	, m_active(false)
{
}

//=================================================================================
void C_UniformBuffer::UploadData(Renderer::I_Renderer& renderer) const
{
	renderer.SetBufferData(m_Handle, GetBufferSize(), Data());
}

//=================================================================================
void C_UniformBuffer::Activate(Renderer::ResourceManager& resourceManager, bool activate)
{
	// bind buffer base here
	auto& glResourceManager = dynamic_cast<GLResourceManager&>(resourceManager);
	if (activate)
	{
		glResourceManager.GetBuffer(m_Handle)->bind();
		m_active = true;
	}
	else
	{
		glResourceManager.GetBuffer(m_Handle)->unbind();
		m_active = false;
	}
}

} // namespace GLEngine::GLRenderer::Buffers