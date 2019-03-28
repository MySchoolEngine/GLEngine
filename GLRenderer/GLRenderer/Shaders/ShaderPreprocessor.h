/** ==============================================
 * @file 		ShaderPreprocessor.h
 * @date 		2018/05/16 21:32
 * @project 	Opengl Shadow Rendering
 * @faculty 	Faculty of Information Technology 
 * @university 	Brno University of Technology
 *
 * @author 		Dominik Rohacek
 * Contact: 	RohacekD@gmail.com
 ** ==============================================*/
 
#pragma once

#include <string>
#include <map>

namespace GLEngine {
namespace GLRenderer {
namespace Shaders {

class C_ShaderPreprocessor {
public:
	void Define(const std::string& symbol, const std::string& value);
	std::string PreprocessFile(const std::string& src, const std::string& filepath);
protected:
	void IncludesFiles(std::string& content, const std::string& filepath);
	void ReplaceConstants(std::string& content);
	void GetDefines(std::string& content);

	bool _loadFile(const char* file, std::string& content);

	using T_Defines = std::map<std::string, std::string>;
	T_Defines m_defines;

	const static std::regex s_IncludeFileName;
};
}
}
}