#pragma once

#include <Renderer/Renderer3D.h>

namespace GLEngine::GLRenderer {
class C_GLRenderInterface : public Renderer::RenderInterface {
public:
	C_GLRenderInterface();
	~C_GLRenderInterface();
	void Render(const Renderer::RenderCall3D& call) override;

private:
	GLuint m_VAOid; // one generic buffer for all calls to ignore VAOs
};
}

