#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderCompiler.h>
#include <GLRenderer/Shaders/ShaderPreprocessor.h>
#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>

namespace GLEngine::GLRenderer::Shaders {

//=================================================================================
bool C_ShaderCompiler::compileShaderStageInternal(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage, std::string& src)
{
	C_ShaderPreprocessor preproces(std::make_unique<C_GLCodeProvider>());
	src = preproces.PreprocessFile(src, filepath.parent_path().generic_string() + "/");
	if (!preproces.WasSuccessful())
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Preprocessing of file '{}' was unsuccessful", filepath.generic_string());
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

	const char* cstr = src.c_str();
	glShaderSource(stage, 1, &cstr, nullptr);
	glCompileShader(stage);

	//Compilation log
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
			CORE_LOG(E_Level::Error, E_Context::Render, "{}", log);

			delete[] log;
		}

		return false;
	}

	return true;
}

//=================================================================================
bool C_ShaderCompiler::linkProgram(T_ShaderProgram& program, const std::vector<T_StageHandle>& shaders)
{
	program = glCreateProgram();

	if (program == 0)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "glCreateProgram failed!");
		return false;
	}

	for (auto & shader : shaders)
	{
		glAttachShader(program, shader);
	}

	glLinkProgram(program);

	//Check link
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

}