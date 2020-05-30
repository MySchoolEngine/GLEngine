#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderCompiler.h>
#include <GLRenderer/Shaders/ShaderPreprocessor.h>

#include <fstream>

namespace GLEngine::GLRenderer::Shaders {
//=================================================================================
C_ShaderCompiler::C_ShaderCompiler(bool preprocessorOutput /*= false*/)
	: m_PreprocessorOutput(preprocessorOutput)
{

}

//=================================================================================
bool C_ShaderCompiler::compileShader(GLuint& shader, const std::filesystem::path& filepath, const GLenum shaderType)
{
	shader = glCreateShader(shaderType);
	if (shader == 0)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "glCreateShader failed!");
		return false;
	}

	std::string src;
	if (!_loadFile(filepath, src))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to open shader file: {}", filepath.generic_string());
		return false;
	}

	C_ShaderPreprocessor preproces;
	src = preproces.PreprocessFile(src, filepath.parent_path().generic_string() + "/");
	if (m_PreprocessorOutput)
	{
		std::ofstream debugOutput;
		const std::filesystem::path debugPath("obj/" + filepath.generic_string() + ".o");
		const auto debugDirectory = debugPath.parent_path();
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
		debugOutput << src;
		debugOutput.flush();
		debugOutput.close();
	}

	if (!preproces.WasSuccessful())
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Preprocessing of file '{}' was unsuccessful", filepath.generic_string());
		return false;
	}

	auto paths = preproces.GetTouchedPaths();
	m_TouchedFiles.insert(m_TouchedFiles.end(), paths.begin(), paths.end());
	m_TouchedFiles.emplace_back(filepath);

	const char* cstr = src.c_str();
	glShaderSource(shader, 1, &cstr, NULL);
	glCompileShader(shader);

	//Compilation log
	GLint result = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		int loglen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &loglen);

		if (loglen > 0)
		{
			char* log = new char[loglen];

			glGetShaderInfoLog(shader, loglen, nullptr, log);
			CORE_LOG(E_Level::Error, E_Context::Render, "\n{}", log);

			delete[] log;
		}

		return false;
	}

	return true;
}

//=================================================================================
bool C_ShaderCompiler::linkProgram(GLuint& program, size_t count, ...)
{
	program = glCreateProgram();

	if (program == 0)
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "glCreateProgram failed!");
		return false;
	}

	va_list args;
	va_start(args, count);
	for (size_t i = 0; i < count; ++i)
	{
		glAttachShader(program, va_arg(args, GLuint));
	}
	va_end(args);

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

//=================================================================================
bool C_ShaderCompiler::linkProgram(GLuint & program, const std::vector<GLuint>& shaders)
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

//=================================================================================
void C_ShaderCompiler::Reset()
{
	m_TouchedFiles.clear();
}

//=================================================================================
C_ShaderCompiler::T_Paths C_ShaderCompiler::GetTouchedFiles() const
{
	return m_TouchedFiles;
}

//=================================================================================
bool C_ShaderCompiler::_loadFile(const std::filesystem::path& file, std::string& content)
{
	std::ifstream stream(file);

	if (stream.fail())
		return false;

	content = std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
	stream.close(); //dr
	return true;
}

}