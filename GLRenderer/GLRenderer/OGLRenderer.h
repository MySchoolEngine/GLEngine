#pragma once

#include <Renderer/IRenderer.h>

#include <Core/CoreMacros.h>

namespace GLEngine {
namespace GLRenderer {

class API_EXPORT C_OGLRenderer : public Renderer::I_Renderer {

public:
	//=================================================================================
	// Renderer::I_Renderer
	//=================================================================================
	virtual void AddCommand(std::unique_ptr<Renderer::I_RenderCommand>) override;
	virtual void AddBatch(std::unique_ptr<Renderer::I_RenderBatch>) override;

	virtual void SortCommands() override;
	virtual void ExtractData() override;
	virtual void TransformData() override;
	virtual void Commit() const override;
	virtual void ClearCommandBuffers() override;

private:

};

}
}