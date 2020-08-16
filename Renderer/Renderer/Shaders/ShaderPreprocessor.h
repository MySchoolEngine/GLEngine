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

#include <Renderer/RendererApi.h>

namespace GLEngine::Renderer::Shaders {

class RENDERER_API_EXPORT C_ShaderPreprocessor {
public:
	using T_Paths = std::vector<std::filesystem::path>;


	C_ShaderPreprocessor(std::unique_ptr<I_CodeProvider>&& codeProvider);
	~C_ShaderPreprocessor();

	void Define(const std::string& symbol, const std::string& value);
	std::string PreprocessFile(const std::string& src, const std::filesystem::path& filepath);
	T_Paths GetTouchedPaths() const;
	bool WasSuccessful() const { return m_Result; }
protected:
	void IncludesFiles(std::string& content, const std::filesystem::path& filepath);
	void CodeGeneration(std::string& content);
	void ReplaceConstants(std::string& content);
	void GetDefines(std::string& content);

	bool _loadFile(const std::filesystem::path& file, std::string& content);

	using T_Defines = std::map<std::string, std::string>;
#pragma warning(push)
#pragma warning( disable : 4251)
	T_Defines m_defines;

	std::unique_ptr<I_CodeProvider> m_CodeProvider;

	T_Paths m_Paths;

	bool m_Result = true;

	const static std::regex s_IncludeFileName;
	const static std::regex s_GenerateStruct;
	const static std::regex s_DefineRegEx;
#pragma warning(pop)
};
}