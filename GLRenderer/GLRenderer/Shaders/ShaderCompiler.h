/** ==============================================
 * @file 		ShaderCompiler.hpp
 * @date 		2018/05/16 21:32
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

namespace GLEngine {
namespace GLRenderer {
namespace Shaders {
class C_ShaderCompiler
{
public:
	using T_Paths = std::vector<std::filesystem::path>;
	//Compiles a single shader from a file
	//errorLog - reference to a string, where error message will be stored, in case the compilation fails
	bool compileShader(GLuint& shader, const std::filesystem::path& filepath, const GLenum shaderType);

	//Links shaders to a program
	//program - reference to a program, into which the shaders will be linked
	//errorLog - error log, if return value is false, contains log data
	//count - number of shaders to link
	//the rest are the shaders, the function has variable amount of arguments
	bool linkProgram(GLuint& program, size_t count, ...);

	//[dr]
	bool linkProgram(GLuint& program, const std::vector<GLuint>& shaders);


	void Reset();
	T_Paths GetTouchedFiles() const;
private:
	bool _loadFile(const std::filesystem::path& file, std::string& content);

	T_Paths m_TouchedFiles;
};
}
}
}