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

#include <QOpenGLFunctions>

#include <fstream>
#include <string>
#include <cstdarg>
#include <sstream>
#include <vector>

class ShaderCompiler
{
public:
	//Compiles a single shader from a file
    //errorLog - reference to a string, where error message will be stored, in case the compilation fails
    bool compileShader(GLuint& shader, const char* filepath, const GLenum shaderType, std::string& errorLog);

    //Links shaders to a program
    //program - reference to a program, into which the shaders will be linked
    //errorLog - error log, if return value is false, contains log data
    //count - number of shaders to link
	//the rest are the shaders, the function has variable amount of arguments
	bool linkProgram(GLuint& program, std::string& errorLog, size_t count, ...);

	//[dr]
	bool linkProgram(GLuint& program, std::string& errorLog, const std::vector<GLuint>& shaders);

private:
    bool _loadFile(const char* file, std::string& content);
};