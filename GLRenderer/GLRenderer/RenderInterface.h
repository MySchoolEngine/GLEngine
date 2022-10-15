#pragma once

namespace GLEngine::Renderer {
class I_DeviceTexture;
}

namespace GLEngine::GLRenderer {
class C_OGLRenderer;
class C_Framebuffer;
namespace Shaders {
class C_ShaderManager;
class C_ShaderProgram;
} // namespace Shaders
namespace Textures {
class C_TextureUnitManger;
}
namespace Mesh {
class C_StaticMeshResource;
}

struct FullScreenSetup {
	std::string									   shaderName;
	std::function<void(Shaders::C_ShaderProgram&)> shaderSetup;
	std::vector<Renderer::I_DeviceTexture*>		   inputTextures;
};
class C_RenderInterface {
public:
	// texture manager to bind textures
	// shader manager
	// renderer to issue commands
	// right now DI just for the form, but I would like to move to DI more
	C_RenderInterface(Shaders::C_ShaderManager& shmgr, Textures::C_TextureUnitManger& tm, C_OGLRenderer& renderer);
	~C_RenderInterface();
	bool RenderFullScreen(const FullScreenSetup& setup);

private:
	Shaders::C_ShaderManager&	   m_shmgr;
	Textures::C_TextureUnitManger& m_tm;
	C_OGLRenderer&				   m_renderer;

	std::shared_ptr<Mesh::C_StaticMeshResource> m_ScreenQuad;
};
} // namespace GLEngine::GLRenderer
