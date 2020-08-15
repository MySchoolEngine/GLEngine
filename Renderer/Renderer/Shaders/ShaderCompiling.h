#pragma once

#include <Renderer/Definitions.h>

namespace GLEngine::Renderer {

template<class StageHandle, class ShaderProgram>
class ShaderCompilerTrait
{
public:
	using T_StageHandle = StageHandle;
	using T_ShaderProgram = ShaderProgram;
	using T_Paths = std::vector<std::filesystem::path>;

	//Compiles a single shader from a file
	//errorLog - reference to a string, where error message will be stored, in case the compilation fails
	bool compileShaderStage(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage);

	T_Paths GetTouchedFiles() const;

	virtual void Reset();
protected:
	bool _loadFile(const std::filesystem::path& file, std::string& content);
	virtual bool compileShaderStageInternal(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage, std::string& content) = 0;

	T_Paths m_TouchedFiles;
};

//=================================================================================
template<class StageHandle, class ShaderProgram>
bool ShaderCompilerTrait<StageHandle, ShaderProgram>::compileShaderStage(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage)
{
	std::string src;
	if (!_loadFile(filepath, src))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to open shader file: {}", filepath.generic_string());
		return false;
	}
	m_TouchedFiles.emplace_back(filepath);

	return compileShaderStageInternal(stage, filepath, shaderStage, src);
}

//=================================================================================
template<class StageHandle, class ShaderProgram>
void ShaderCompilerTrait<StageHandle, ShaderProgram>::Reset()
{
	m_TouchedFiles.clear();
}

//=================================================================================
template<class StageHandle, class ShaderProgram>
typename ShaderCompilerTrait<StageHandle, ShaderProgram>::T_Paths ShaderCompilerTrait<StageHandle, ShaderProgram>::GetTouchedFiles() const
{
	return m_TouchedFiles;
}

//=================================================================================
template<class StageHandle, class ShaderProgram>
bool ShaderCompilerTrait<StageHandle, ShaderProgram>::_loadFile(const std::filesystem::path& file, std::string& content)
{
	std::ifstream stream(file);

	if (stream.fail())
		return false;

	content = std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
	stream.close(); //dr
	return true;
}

}

