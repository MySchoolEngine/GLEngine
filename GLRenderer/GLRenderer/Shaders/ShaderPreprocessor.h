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

#include <Renderer/Shaders/CodeGeneration.h>

namespace GLEngine::GLRenderer::Shaders {

class C_ShaderPreprocessor {
public:
	C_ShaderPreprocessor(std::unique_ptr<Renderer::Shaders::I_CodeProvider>&& codeProvider);

	using T_Paths = std::vector<std::filesystem::path>;
	void Define(const std::string& symbol, const std::string& value);
	std::string PreprocessFile(const std::string& src, const std::string& filepath);
	T_Paths GetTouchedPaths() const;
	bool WasSuccessful() const { return m_Result; }
protected:
	void IncludesFiles(std::string& content, const std::string& filepath);
	void CodeGeneration(std::string& content);
	void ReplaceConstants(std::string& content);
	void GetDefines(std::string& content);

	bool _loadFile(const char* file, std::string& content);

	using T_Defines = std::map<std::string, std::string>;
	T_Defines m_defines;

	std::unique_ptr<Renderer::Shaders::I_CodeProvider> m_CodeProvider;

	T_Paths m_Paths;

	bool m_Result = true;

	const static std::regex s_IncludeFileName;
	const static std::regex s_GenerateStruct;
	const static std::regex s_DefineRegEx;
};
}