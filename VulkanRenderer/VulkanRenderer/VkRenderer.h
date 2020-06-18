#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderer.h>

namespace GLEngine::VkRenderer {

class C_VkRenderer : public Renderer::I_Renderer {
public:
	C_VkRenderer();
	virtual ~C_VkRenderer() override;

	//=================================================
	// Renderer::I_Renderer
	//=================================================
	virtual void Lock(bool lock = true) override;
	virtual void AddCommand(T_CommandPtr) override;
	virtual void AddBatch(T_BatchPtr) override;
	virtual void SortCommands() override;
	virtual void ExtractData() override;
	virtual void TransformData() override;
	virtual void Commit() const override;
	virtual void ClearCommandBuffers() override;

private:
	bool m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>* m_CommandQueue;
};
}