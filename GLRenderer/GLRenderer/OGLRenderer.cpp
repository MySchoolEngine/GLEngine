#include <GLRenderer/OGLRenderer.h>

#include <Core/CoreMacros.h>

#include <Utils/Logging/LoggingMacros.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>

namespace GLEngine {
namespace GLRenderer {

//=================================================================================
C_OGLRenderer::C_OGLRenderer()
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
{
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (status == 0) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Glad wasn't loaded properlly. Status {}", status);
	}
}

//=================================================================================
C_OGLRenderer::~C_OGLRenderer()
{
	delete m_CommandQueue;
}

//=================================================================================
void C_OGLRenderer::AddCommand(Renderer::I_Renderer::T_CommandPtr command)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_OGLRenderer::AddBatch(Renderer::I_Renderer::T_BatchPtr batc)
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_OGLRenderer::SortCommands()
{
	// todo: no sorting for now
}

//=================================================================================
void C_OGLRenderer::ExtractData()
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_OGLRenderer::TransformData()
{
	// todo: no extraction for now
}

//=================================================================================
void C_OGLRenderer::Commit() const
{
	throw std::logic_error("The method or operation is not implemented.");
}

//=================================================================================
void C_OGLRenderer::ClearCommandBuffers()
{
	throw std::logic_error("The method or operation is not implemented.");
}

}
}

