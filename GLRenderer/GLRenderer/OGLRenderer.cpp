#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Debug.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/OGLDevice.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Textures/TextureManager.h>

#include <Renderer/IRenderBatch.h>
#include <Renderer/IRenderCommand.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>
#include <GUI/Menu/MenuItem.h>

#include <Utils/DebugBreak.h>
#include <Utils/EnumUtils.h>

#include <imgui.h>
#include <stdexcept>

namespace GLEngine::GLRenderer {

//=================================================================================
C_OGLRenderer::C_OGLRenderer(C_GLDevice& device)
	: m_CommandQueue(new std::remove_pointer<decltype(m_CommandQueue)>::type)
	, m_DrawCommands("Draw commands")
	, m_CatchErrors(false, "Catch errors")
	, m_Wireframe(false, "Render wireframe")
	, m_PreviousCatchErrorsVal(false)
	, m_CurrentPass(Renderer::E_PassType::FinalPass)
	, m_GUITexts({{GUI::C_FormatedText("Avg draw commands: {:.2f}"), GUI::C_FormatedText("Min/max {:.2f}/{:.2f}"), GUI::C_FormatedText("Draw calls: {}"),
				   GUI::C_FormatedText("UBO memory usage: {}B")}})
	, m_ScreenCaptureList("Capture frame commands", [&]() { m_OutputCommandList = true; })
	, m_Window(GUID::INVALID_GUID)
	, m_Windows(std::string("Windows"))
	, m_Device(device)
{
	if (m_CatchErrors)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(MessageCallback, nullptr);
	}

	auto& tmgr = Textures::C_TextureManager::Instance(&device);
}

//=================================================================================
C_OGLRenderer::~C_OGLRenderer()
{
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	shmgr.Clear();
	auto& tmgr = Textures::C_TextureManager::Instance();
	tmgr.Clear();
	delete m_CommandQueue;
}

//=================================================================================
void C_OGLRenderer::AddCommand(Renderer::I_Renderer::T_CommandPtr command)
{
	if (m_Locked)
	{
		GL_DebugBreak();
	}
	// must be after m_Locked check, otherwise will cause deadlock for any broken lambda command
	std::lock_guard<std::mutex> guardCommand(m_CommandQueueMTX);
	m_CommandQueue->emplace_back(std::move(command));
}

//=================================================================================
void C_OGLRenderer::AddTransferCommand(T_CommandPtr command)
{
	std::lock_guard<std::mutex> guardTransfer(m_TransferQueueMTX);
	m_TransferQueue.emplace_back(std::move(command));
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
	{
		std::lock_guard<std::mutex> guard(m_TransferQueueMTX);
		for (auto& command : m_TransferQueue)
		{
			command->Commit();
		}
	}
	{
		std::lock_guard<std::mutex> guard(m_CommandQueueMTX);
		for (auto& command : (*m_CommandQueue))
		{
			command->Commit();
		}
	}
}

//=================================================================================
void C_OGLRenderer::ClearCommandBuffers()
{
	// TODO: this is still not safe, some command can come in between commit and this point
	std::lock_guard<std::mutex> guardCommand(m_CommandQueueMTX);
	std::lock_guard<std::mutex> guardTransfer(m_TransferQueueMTX);
	m_DrawCommands.Sample(static_cast<float>(m_CommandQueue->size()));
	const auto drawCallsNum
		= std::count_if(m_CommandQueue->begin(), m_CommandQueue->end(), [](const auto& command) { return command->GetType() == Renderer::I_RenderCommand::E_Type::DrawCall; });
	if (m_OutputCommandList)
	{
		CaputreCommands();
		m_OutputCommandList = false;
	}
	m_CommandQueue->clear();
	m_TransferQueue.clear();
	const auto avgDrawCommands = m_DrawCommands.Avg();
	m_GUITexts[Utils::ToIndex(E_GUITexts::AvgDrawCommands)].UpdateText(avgDrawCommands);
	m_GUITexts[Utils::ToIndex(E_GUITexts::MinMax)].UpdateText(*std::min_element(m_DrawCommands.cbegin(), m_DrawCommands.cend()),
																							   *std::max_element(m_DrawCommands.cbegin(), m_DrawCommands.cend()));
	m_GUITexts[Utils::ToIndex(E_GUITexts::DrawCalls)].UpdateText(drawCallsNum);
	m_GUITexts[Utils::ToIndex(E_GUITexts::UBOMemoryUsage)].UpdateText(Buffers::C_UniformBuffersManager::Instance().GetUsedMemory());

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
GUID C_OGLRenderer::SetupControls(GUI::C_GUIManager& guiMan)
{
	m_Window		  = guiMan.CreateGUIWindow("Renderer frame stats");
	auto* renderStats = guiMan.GetWindow(m_Window);
	renderStats->AddComponent(m_DrawCommands);
	renderStats->AddComponent(m_CatchErrors);
	renderStats->AddComponent(m_Wireframe);
	renderStats->AddComponent(m_GUITexts[Utils::ToIndex(E_GUITexts::AvgDrawCommands)]);
	renderStats->AddComponent(m_GUITexts[Utils::ToIndex(E_GUITexts::MinMax)]);
	renderStats->AddComponent(m_GUITexts[Utils::ToIndex(E_GUITexts::DrawCalls)]);
	renderStats->AddComponent(m_GUITexts[Utils::ToIndex(E_GUITexts::UBOMemoryUsage)]);

	renderStats->AddMenu(m_Windows);

	auto&	   shmgr	   = Shaders::C_ShaderManager::Instance();
	const auto shmgrWindow = shmgr.SetupControls(guiMan);

	auto&	   tmgr		  = Textures::C_TextureManager::Instance();
	const auto tmgrWindow = tmgr.SetupControls(guiMan);

	m_Windows.AddMenuItem(guiMan.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Shader manager", shmgrWindow, guiMan));
	m_Windows.AddMenuItem(guiMan.CreateMenuItem<GUI::Menu::C_MenuItemOpenWindow>("Texture manager", tmgrWindow, guiMan));
	renderStats->AddComponent(m_ScreenCaptureList);

	return m_Window;
}

//=================================================================================
void C_OGLRenderer::DestroyControls(GUI::C_GUIManager& guiMan)
{
	guiMan.DestroyWindow(m_Window);
	auto& shmgr = Shaders::C_ShaderManager::Instance();
	shmgr.DestroyControls(guiMan);

	auto& tmgr = Textures::C_TextureManager::Instance();
	tmgr.DestroyControls(guiMan);
}

//=================================================================================
Renderer::E_PassType C_OGLRenderer::GetCurrentPassType() const
{
	return m_CurrentPass;
}

//=================================================================================
void C_OGLRenderer::SetCurrentPassType(Renderer::E_PassType type)
{
	m_CurrentPass = type;
}

//=================================================================================
void C_OGLRenderer::CaputreCommands() const
{
	std::ofstream				file;
	const std::filesystem::path debugPath("obj/frameCommands.txt");
	file.open(debugPath);

	if (!file.is_open())
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Cannot open file for debug output");
	}
	file << "Transfer queue\n";
	for (const auto& command : m_TransferQueue)
	{
		file << command->GetDescriptor() << "\n";
	}
	file << "Command queue\n";
	for (const auto& command : (*m_CommandQueue))
	{
		file << command->GetDescriptor() << "\n";
	}
	file.flush();
	file.close();
}

//=================================================================================
GLEngine::Renderer::I_Device& C_OGLRenderer::GetDevice()
{
	return m_Device;
}

} // namespace GLEngine::GLRenderer
