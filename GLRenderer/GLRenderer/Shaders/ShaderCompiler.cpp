#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderCompiler.h>
#include <GLRenderer/Shaders/ShaderPreprocessor.h>

namespace GLEngine {
namespace GLRenderer {
namespace Shaders {
//=================================================================================
const std::regex C_ShaderCompiler::s_reg = std::regex(R"(^(.+\/)*(.+)\.(.+)$)");

//=================================================================================
bool C_ShaderCompiler::compileShader(GLuint& shader, const char* filepath, const GLenum shaderType)
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
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to open shader file: {}", std::string(filepath));
		return false;
	}

	std::smatch m;

	std::filesystem::path filestr(filepath);

	C_ShaderPreprocessor preproces;
	src = preproces.PreprocessFile(src, filestr.parent_path().generic_string() + "/");
	if (!preproces.WasSuccessful())
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Preprocessing of file '{}' was unsuccessful", filepath);
		return false;
	}

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
			CORE_LOG(E_Level::Error, E_Context::Render, log);

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
bool C_ShaderCompiler::_loadFile(const char* file, std::string& content)
{
	std::ifstream stream(file);

	if (stream.fail())
		return false;

	content = std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
	stream.close(); //dr
	return true;
}

}
}
}