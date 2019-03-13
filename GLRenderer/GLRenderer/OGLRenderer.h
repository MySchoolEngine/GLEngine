#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderer.h>


#include <vector>

namespace GLEngine {
namespace GLRenderer {

class API_EXPORT C_OGLRenderer : public Renderer::I_Renderer {
public:
	C_OGLRenderer();
	virtual ~C_OGLRenderer() override;

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
	std::vector<Renderer::I_Renderer::T_CommandPtr>* m_CommandQueue;
};

}
}