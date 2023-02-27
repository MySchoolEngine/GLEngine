#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>
#include <GLRenderer/Shaders/ShaderCompiler.h>

#include <Renderer/Shaders/ShaderPreprocessor.h>

#include <fstream>

namespace GLEngine::GLRenderer::Shaders {
//=================================================================================
C_ShaderCompiler::C_ShaderCompiler(bool preprocessorOutput /*= false*/)
	: m_PreprocessorOutput(preprocessorOutput)
{
}

//=================================================================================
bool C_ShaderCompiler::compileShaderStageInternal(T_StageHandle&				stage,
												  const std::filesystem::path&	filepath,
												  const Renderer::E_ShaderStage shaderStage,
												  std::vector<char>&			content,
												  const std::string&			entryPoint)
{
	// todo: entryPoint
	Renderer::Shaders::C_ShaderPreprocessor preproces(std::make_unique<C_GLCodeProvider>());
	std::string								strContent(content.begin(), content.end());
	strContent = preproces.PreprocessFile(strContent, filepath.parent_path());
	if (m_PreprocessorOutput)
	{
		std::ofstream				debugOutput;
		const std::filesystem::path debugPath("obj/" + filepath.generic_string() + ".o");
		const auto					debugDirectory = debugPath.parent_path();
		if (!std::filesystem::exists(debugDirectory))
		{
			if (!std::filesystem::create_directories(debugDirectory))
			{
				CORE_LOG(E_Level::Error, E_Context::Render, "Cannot create debug output directory");
			}
		}
		debugOutput.open(debugPath);
		if (!debugOutput.is_open())
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Cannot open file for debug output");
		}
		debugOutput << strContent;
		debugOutput.flush();
		debugOutput.close();
	}

	if (!preproces.WasSuccessful())
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Preprocessing of file '{}' was unsuccessful", filepath);
		return false;
	}

	auto paths = preproces.GetTouchedPaths();
	m_TouchedFiles.insert(m_TouchedFiles.end(), paths.begin(), paths.end());

	stage = glCreateShader(ShaderStageTypeToEnum(shaderStage));
	if (stage == 0)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "glCreateShader failed!");
		return false;
	}

	const char* cstr = strContent.c_str();
	glShaderSource(stage, 1, &cstr, nullptr);
	glCompileShader(stage);

	// Compilation log
	GLint result = 0;
	glGetShaderiv(stage, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int loglen = 0;
		glGetShaderiv(stage, GL_INFO_LOG_LENGTH, &loglen);

		if (loglen > 0)
		{
			char* log = new char[loglen];

			glGetShaderInfoLog(stage, loglen, nullptr, log);
			CORE_LOG(E_Level::Error, E_Context::Render, "\n{}", log);

			delete[] log;
		}

		return false;
	}

	return true;
}

//=================================================================================
bool C_ShaderCompiler::linkProgram(GLuint& program, const std::vector<std::pair<Renderer::E_ShaderStage, GLuint>>& stages)
{
	program = glCreateProgram();

	if (program == 0)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "glCreateProgram failed!");
		return false;
	}

	for (auto& shader : stages)
	{
		glAttachShader(program, shader.second);
	}

	glLinkProgram(program);

	// Check link
	int status = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		int loglen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &loglen);

		if (loglen > 0)
		{
			char* log = new char[loglen];

			glGetProgramInfoLog(program, loglen, nullptr, log);
			CORE_LOG(E_Level::Error, E_Context::Render, log);

			delete[] log;
		}

		return false;
	}

	return true;
}

//=================================================================================
void C_ShaderCompiler::ReleaseStage(T_StageHandle& stage)
{
	glDeleteShader(stage);
}

} // namespace GLEngine::GLRenderer::Shaders