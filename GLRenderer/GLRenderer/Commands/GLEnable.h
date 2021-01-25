#pragma once

#include <Renderer/IRenderCommand.h>

#include <Core/CoreMacros.h>

#include <Utils/BitField.h>

namespace GLEngine { namespace GLRenderer { namespace Commands {

class C_GLEnable : public Renderer::I_RenderCommand {
public:
	enum class E_GLEnableValues
	{
		DEPTH_TEST		  = GL_DEPTH_TEST,
		CULL_FACE		  = GL_CULL_FACE,
		PRIMITIVE_RESTART = GL_PRIMITIVE_RESTART,
		BLEND			  = GL_BLEND,
		CLIPPLANE0		  = GL_CLIP_PLANE0,
		CLIPPLANE1		  = GL_CLIP_PLANE1,
		CLIPPLANE2		  = GL_CLIP_PLANE2,
		CLIPPLANE3		  = GL_CLIP_PLANE3,
		CLIPPLANE4		  = GL_CLIP_PLANE4,
		CLIPPLANE5		  = GL_CLIP_PLANE5,
	};

	explicit C_GLEnable(E_GLEnableValues value);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	virtual void								  Commit() override;
	virtual E_Type								  GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override { return nullptr; };
	[[nodiscard]] virtual std::string			  GetDescriptor() const override;

protected:
	E_GLEnableValues m_Value;
};

}}} // namespace GLEngine::GLRenderer::Commands