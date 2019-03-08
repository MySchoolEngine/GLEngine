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
	virtual void AddCommand(Renderer::I_Renderer::T_CommandPtr) override;
	virtual void AddBatch(Renderer::I_Renderer::T_BatchPtr) override;

	virtual void SortCommands() override;
	virtual void ExtractData() override;
	virtual void TransformData() override;
	virtual void Commit() const override;
	virtual void ClearCommandBuffers() override;

private:

};

}
}