#pragma once

#include <Renderer/ComputeRenderer.h>

namespace GLEngine::GLRenderer {
class C_GLRendererComputeInterface : public Renderer::I_ComputeRenderInterface {
public:
	void Render(const Renderer::ComputeRenderCall& call) override;
};
} //~GLEngine::GLRenderer