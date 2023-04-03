#include <GLRendererStdafx.h>

#include <GLRenderer/GLResourceManager.h>

namespace GLEngine::GLRenderer {

//=================================================================================
Renderer::Handle<Renderer::Texture> GLResourceManager::createTexture(const Renderer::TextureDescriptor& desc)
{
	return {};
//	m_TexturePool.CreateNew(desc);
}

}
