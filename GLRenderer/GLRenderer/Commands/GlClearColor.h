#pragma once

#include <Core/CoreMacros.h>

#include <Renderer/IRenderCommand.h>

#include <glm/vec4.hpp>
#include <glm/vec3.hpp>

namespace GLEngine {
namespace GLRenderer {
namespace Commands {

class C_GLClearColor : public Renderer::I_RenderCommand {
public:
	explicit C_GLClearColor(glm::vec4&& color);
	explicit C_GLClearColor(glm::vec3 color);

	//===========================================
	// Renderer::I_RenderCommand
	//===========================================
	virtual void Commit() override;
	virtual E_Type GetType() const override;
	virtual std::shared_ptr<Renderer::I_Resource> GetResource() const override;


protected:
	glm::vec4 m_Colour;
};

}}}