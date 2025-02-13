#include <GLRendererStdafx.h>

#include <GLRenderer/Buffers/UniformBuffersManager.h>
#include <GLRenderer/Shaders/ShaderCompiler.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Shaders/ShaderProgram.h>

#include <Renderer/Shaders/ShaderCompiling.h>

#include <GUI/GUIManager.h>
#include <GUI/GUIWindow.h>

#include <imgui.h>

namespace GLEngine::GLRenderer::Shaders {
//=================================================================================
const std::filesystem::path C_ShaderManager::s_ShadersFolder = "shaders/";

//=================================================================================
C_ShaderManager::C_ShaderManager()
	: m_Timeout(std::chrono::seconds(1))
	, m_LastUpdate(std::chrono::system_clock::now())
	, m_Window(GUID::INVALID_GUID)
	, m_PreprocessorOutput(false, "Output preprocessed")
{
}

//=================================================================================
C_ShaderManager& C_ShaderManager::Instance()
{
	static C_ShaderManager instance; // Guaranteed to be destroyed.
									 // Instantiated on first use.
	return instance;
}

//=================================================================================
void C_ShaderManager::Clear()
{
	CORE_LOG(E_Level::Info, E_Context::Render, "{}", ShadersStatistics());
	std::cout << ShadersStatistics();
	m_Programs.clear();
	m_ActiveShader.reset();
}

//=================================================================================
void C_ShaderManager::Update()
{
	DeactivateShader();
#ifdef GL_ENGINE_DEBUG
	const auto currentTime = std::chrono::system_clock::now();
	if (m_LastUpdate + m_Timeout < currentTime)
	{
		m_ActiveShader.reset();
		for (auto& [name, shader] : m_Programs)
		{
			if (!shader->IsExpired())
			{
				continue;
			}

			ReloadProgram(name, shader);
		}
		m_LastUpdate = currentTime;
	}
#endif
}

//=================================================================================
C_ShaderManager::T_ShaderPtr C_ShaderManager::GetProgram(const std::string& name)
{
	if (ShaderLoaded(name))
	{
		return m_Programs[name];
	}

	C_ShaderCompiler compiler(m_PreprocessorOutput.GetValue());
	GLuint			 program = LoadProgram(name, compiler);
	if (program == 0)
	{
		return nullptr;
	}

	T_ShaderPtr shaderProgram = std::make_shared<C_ShaderProgram>(program);
	shaderProgram->SetName(name);
#if _DEBUG
	shaderProgram->SetPaths(compiler.GetTouchedFiles());
#endif

	Buffers::C_UniformBuffersManager::Instance().ProcessUBOBindingPoints(shaderProgram);

	m_Programs.insert({name, shaderProgram});

	return shaderProgram;
}

//=================================================================================
bool C_ShaderManager::ShaderLoaded(const std::string& name) const
{
	return m_Programs.contains(name);
}

//=================================================================================
void C_ShaderManager::ActivateShader(const T_ShaderPtr& shader)
{
	if (shader == nullptr)
	{
		throw 1; // just error
	}
	if (shader != m_ActiveShader || !m_ActiveShader->IsActive())
	{
		m_ActiveShader = shader;
		m_ActiveShader->useProgram();
	}
}

//=================================================================================
void C_ShaderManager::DeactivateShader()
{
	// this is done because imGUI can change shader without letting us know
	if (m_ActiveShader)
	{
		m_ActiveShader->disableProgram();
		m_ActiveShader = nullptr;
	}
}

//=================================================================================
std::string C_ShaderManager::ShadersStatistics() const
{
	std::stringstream ss;
	ss << "Loaded shaders: " << m_Programs.size() << std::endl;
	for (const auto& [name, shader] : m_Programs)
	{
		ss << name << " used by " << shader.use_count() - 1 << std::endl;
		ss << "->\tHas stages:" << std::endl;
	}
	return ss.str();
}

//=================================================================================
GUID C_ShaderManager::SetupControls(GUI::C_GUIManager& guiMGR)
{
	m_Window		= guiMGR.CreateGUIWindow("Shader manager");
	auto* shaderMan = guiMGR.GetWindow(m_Window);

	m_ShaderList = std::make_unique<GUI::C_LambdaPart>([&]() {
		for (const auto& program : m_Programs)
		{
			bool selected = false;
			::ImGui::Selectable(program.first.c_str(), &selected);
			if (selected)
			{
				ReloadProgram(program.first, program.second);
			}
		}
		return false; // nothing to be saved here
	});

	shaderMan->AddComponent(m_PreprocessorOutput);
	shaderMan->AddComponent(*m_ShaderList.get());

	return m_Window;
}

//=================================================================================
void C_ShaderManager::DestroyControls(GUI::C_GUIManager& guiMGR)
{
	guiMGR.DestroyWindow(m_Window);
}

//=================================================================================
GLuint C_ShaderManager::LoadProgram(const std::filesystem::path& name, C_ShaderCompiler& compiler) const
{
	Renderer::C_ShaderLoader<GLuint> loader(compiler);

	std::vector<std::pair<Renderer::E_ShaderStage, GLuint>> stages;
	loader.LoadAllStages(name, stages);

	GLuint program;
	if (!compiler.linkProgram(program, stages))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Problem occured during load of shader pipeline: {}", name);
		return false;
	}
	return program;
}

//=================================================================================
void C_ShaderManager::ReloadProgram(const std::string& programName, const std::shared_ptr<C_ShaderProgram>& program) const
{
	C_ShaderCompiler compiler(m_PreprocessorOutput.GetValue());
	try
	{
		*(program) = std::move(C_ShaderProgram(LoadProgram(std::filesystem::path(programName), compiler)));
		program->SetName(programName);
#if _DEBUG
		program->SetPaths(compiler.GetTouchedFiles());
#endif
		Buffers::C_UniformBuffersManager::Instance().ProcessUBOBindingPoints(program);
	} catch (...)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Unable to reload shader {} so we keep it outdated", programName);
	}
}
} // namespace GLEngine::GLRenderer::Shaders