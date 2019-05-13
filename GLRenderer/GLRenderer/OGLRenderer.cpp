#include <GLRendererStdafx.h>

#include <GLRenderer/OGLRenderer.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

#include <imgui.h>

#include <stdexcept>

namespace GLEngine {
namespace GLRenderer {

//=================================================================================
C_OGLRenderer::C_OGLRenderer()
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
	, m_DrawCommands("Draw commands")
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
	if (m_Locked) {
		__debugbreak();
	}
	m_CommandQueue->emplace_back(std::move(command));
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
	for (auto& command : (*m_CommandQueue)) {
		command->Commit();
	}
}

//=================================================================================
void C_OGLRenderer::ClearCommandBuffers()
{
	m_DrawCommands.Sample(static_cast<float>(m_CommandQueue->size()));
	m_CommandQueue->clear();
}

//=================================================================================
void C_OGLRenderer::Lock(bool lock /*= true*/)
{
	m_Locked = lock;
}

//=================================================================================
void C_OGLRenderer::DrawControls() const
{
	bool my_tool_active = true;
	
	::ImGui::Begin("Renderer frame stats", &my_tool_active);
		m_DrawCommands.Draw();
		const auto avgDrawCommands = m_DrawCommands.Avg();
		::ImGui::Text("Avg draw commands: %f.2", avgDrawCommands);
	::ImGui::End();
}

}}

