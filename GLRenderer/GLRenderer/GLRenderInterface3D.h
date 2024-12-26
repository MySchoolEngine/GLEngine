#pragma once

#include <Renderer/Renderer3D.h>

namespace GLEngine::GLRenderer {
class C_GLRenderInterface3D : public Renderer::I_RenderInterface3D {
public:
	C_GLRenderInterface3D();
	~C_GLRenderInterface3D() override;
	void Render(const Renderer::RenderCall3D& call) override;

private:
	GLuint m_VAOid; // one generic buffer for all calls to ignore VAOs
};
}

