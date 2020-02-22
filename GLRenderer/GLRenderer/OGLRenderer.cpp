#include <GLRendererStdafx.h>

#include <GLRenderer/OGLRenderer.h>

#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/ImGui/GUIManager.h>
#include <GLRenderer/GUI/GUIWindow.h>
#include <GLRenderer/GUI/Menu/MenuItem.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

#include <GLRenderer/Debug.h>

#include <imgui.h>

#include <stdexcept>

namespace GLEngine {
namespace GLRenderer {

//=================================================================================
C_OGLRenderer::C_OGLRenderer()
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
	, m_DrawCommands("Draw commands")
	, m_CatchErrors(false, "Catch errors")
	, m_PreviousCatchErrorsVal(false)
	, m_GUITexts(
		{{
				("Avg draw commands: {:.2f}"),
				("Min/max {:.2f}/{:.2f}")
			}})
	, m_Window(INVALID_GUID)
	, m_Windows("Windows")
{
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	if (status == 0) {
		CORE_LOG(E_Level::Error, E_Context::Core, "GLFW: Glad wasn't loaded properlly. Status {}", status);
	}
}

//=================================================================================
C_OGLRenderer::~C_OGLRenderer()
{
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	shmgr.Clear();
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
	const auto avgDrawCommands = m_DrawCommands.Avg();
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgDrawCommands)].UpdateText(avgDrawCommands);
	m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::MinMax)]
		.UpdateText(
			*std::min_element(m_DrawCommands.cbegin(), m_DrawCommands.cend()),
			*std::max_element(m_DrawCommands.cbegin(), m_DrawCommands.cend()));


	if (m_PreviousCatchErrorsVal != m_CatchErrors)
	{
		if (m_CatchErrors)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(MessageCallback, 0);
		}
		else
		{
			glDisable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(nullptr, 0);
		}
		m_PreviousCatchErrorsVal = m_CatchErrors.GetValue();
	}
}

//=================================================================================
void C_OGLRenderer::Lock(bool lock /*= true*/)
{
	m_Locked = lock;
}

//=================================================================================
GUID C_OGLRenderer::SetupControls(ImGui::C_GUIManager& guiMan)
{
	m_Window = guiMan.CreateGUIWindow("Renderer frame stats");
	auto* renderStats = guiMan.GetWindow(m_Window);
	renderStats->AddComponent(m_DrawCommands);
	renderStats->AddComponent(m_CatchErrors);
	renderStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::AvgDrawCommands)]);
	renderStats->AddComponent(m_GUITexts[static_cast<std::underlying_type_t<E_GUITexts>>(E_GUITexts::MinMax)]);

	renderStats->AddMenu(m_Windows);
	
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	const auto shmgrWindwo = shmgr.SetupControls(guiMan);

	m_ShaderMGR = std::make_unique<GUI::Menu::C_MenuItemOpenWindow>("Shader manager", shmgrWindwo, guiMan);

	m_Windows.AddMenuItem(*m_ShaderMGR.get());

	return m_Window;
}

//=================================================================================
void C_OGLRenderer::DestroyControls(ImGui::C_GUIManager& guiMan)
{
	guiMan.DestroyWindow(m_Window);
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	shmgr.DestroyControls(guiMan);
}

}}

