#include <GLRendererStdafx.h>

#include <GLRenderer/Shaders/ShaderCompiler.h>
#include <GLRenderer/Shaders/ShaderPreprocessor.h>

namespace GLEngine {
namespace GLRenderer {
namespace Shaders {
//=================================================================================
const std::regex C_ShaderCompiler::s_reg = std::regex(R"(^(.+\/)*(.+)\.(.+)$)");

//=================================================================================
bool C_ShaderCompiler::compileShader(GLuint& shader, const char* filepath, const GLenum shaderType, std::string& errorLog)
{
	shader = glCreateShader(shaderType);
	if (shader == 0)
	{
		errorLog = "glCreateShader failed\n";
		return false;
	}

	std::string src;


	if (!_loadFile(filepath, src))
	{
		std::stringstream sstr;
		sstr << "Failed to open shader file: " << std::string(filepath) << std::endl;
		errorLog = sstr.str();
		return false;
	}

	std::smatch m;

	std::string filestr(filepath);

	if (std::regex_search(filestr, m, s_reg)) {
		C_ShaderPreprocessor preproces;
		src = preproces.PreprocessFile(src, m[1].str());
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
			errorLog.append(log);

			delete[] log;
		}

		return false;
	}

	return true;
}

//=================================================================================
bool C_ShaderCompiler::linkProgram(GLuint& program, std::string& errorLog, size_t count, ...)
{
	program = glCreateProgram();

	if (program == 0)
	{
		errorLog = "glCreateProgram failed!\n";
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
			errorLog.assign(log);

			delete[] log;
		}

		return false;
	}

	return true;
}

//=================================================================================
bool C_ShaderCompiler::linkProgram(GLuint & program, std::string & errorLog, const std::vector<GLuint>& shaders)
{
	program = glCreateProgram();

	if (program == 0)
	{
		errorLog = "glCreateProgram failed!\n";
		return false;
	}

	for (auto & shader : shaders)
	{
		glAttachShader(program, shader);
		//ErrorCheck();
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
			errorLog.assign(log);

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