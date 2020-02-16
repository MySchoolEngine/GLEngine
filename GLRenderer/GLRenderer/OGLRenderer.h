#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderer.h>

#include <GLRenderer/GUI/PlotLine.h>
#include <GLRenderer/GUI/Input/CheckBoxValue.h>

namespace GLEngine {
namespace GLRenderer {

class C_OGLRenderer : public Renderer::I_Renderer {
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


	//=================================================================================
	virtual void Lock(bool lock = true) override;

	void DrawControls() const;
private:
	bool m_Locked = false;
	std::vector<Renderer::I_Renderer::T_CommandPtr>*	m_CommandQueue;
	GUI::C_PlotLine<500>								m_DrawCommands;
	GUI::Input::C_CheckBoxValue					m_CatchErrors;
	bool																m_PreviousCatchErrorsVal;
};

}
}