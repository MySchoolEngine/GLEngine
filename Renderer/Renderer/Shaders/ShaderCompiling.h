#pragma once

#include <Renderer/Definitions.h>

#include <Utils/Logging/LoggingMacros.h>

#include <pugixml.hpp>

namespace GLEngine::Renderer {

//=================================================================================
template <class StageHandle> class ShaderCompilerTrait {
public:
	using T_StageHandle = StageHandle;
	using T_Paths		= std::vector<std::filesystem::path>;

	virtual ~ShaderCompilerTrait() = default;

	// Compiles a single shader from a file
	// errorLog - reference to a string, where error message will be stored, in
	// case the compilation fails
	bool		 compileShaderStage(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage);
	virtual void ReleaseStage(T_StageHandle& stage) = 0;

	T_Paths GetTouchedFiles() const;

	virtual void Reset();

protected:
	bool		 _loadFile(const std::filesystem::path& file, std::vector<char>& content);
	virtual bool compileShaderStageInternal(T_StageHandle&				  stage,
											const std::filesystem::path&  filepath,
											const Renderer::E_ShaderStage shaderStage,
											std::vector<char>&			  content,
											const std::string&			  entryPoint)
		= 0;

	T_Paths m_TouchedFiles;
};

//=================================================================================
template <class StageHandle> class C_ShaderLoader {
public:
	using T_ShaderStage = std::pair<E_ShaderStage, StageHandle>;
	C_ShaderLoader(ShaderCompilerTrait<StageHandle>& compiler)
		: m_Compiler(compiler)
	{
	}
	virtual ~C_ShaderLoader() = default;

	bool LoadAllStages(const std::filesystem::path& name, std::vector<T_ShaderStage>& stages)
	{
		pugi::xml_document doc;

		if (!LoadDoc(doc, name))
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Can't open config file for shader name: {}", name);
			return false;
		}

		for (auto& shader : doc.child("pipeline").children("shader"))
		{
			T_ShaderStage shaderStage;
			if (!LoadShaderStage(shader, shaderStage))
			{
				return false;
			}
			stages.push_back(shaderStage);
		}

		return true;
	}

private:
	const static std::filesystem::path s_ShadersFolder;
	ShaderCompilerTrait<StageHandle>&  m_Compiler;

	//=================================================================================
	bool LoadShaderStage(const pugi::xml_node& node, T_ShaderStage& stagePair)
	{
		StageHandle shader;

		const std::string_view stageAttribute = node.attribute("stage").value();

		Renderer::E_ShaderStage stage = Renderer::E_ShaderStage::Vertex;
		if (stageAttribute == "vertex")
			stage = Renderer::E_ShaderStage::Vertex;
		else if (stageAttribute == "fragment")
			stage = Renderer::E_ShaderStage::Fragment;
		else if (stageAttribute == "geometry")
			stage = Renderer::E_ShaderStage::Geometry;
		else if (stageAttribute == "compute")
			stage = Renderer::E_ShaderStage::Compute;
		else if (stageAttribute == "tess-control")
			stage = Renderer::E_ShaderStage::TesselationControl;
		else if (stageAttribute == "tess-evaluation")
			stage = Renderer::E_ShaderStage::TesselationEvaluation;

		const auto filename = s_ShadersFolder / std::filesystem::path(node.first_child().value());

		if (!m_Compiler.compileShaderStage(shader, filename, stage))
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "--Compilation error");
			CORE_LOG(E_Level::Error, E_Context::Render, "{}", filename);
			m_Compiler.ReleaseStage(shader);
			return false;
		}
		stagePair = {stage, shader};
		return true;
	}

	//=================================================================================
	bool LoadDoc(pugi::xml_document& document, const std::filesystem::path& filename) const
	{
		pugi::xml_parse_result result;

		if (filename.has_extension())
		{
			result = document.load_file(filename.c_str());
			if (!result.status == pugi::status_ok)
			{
				const std::filesystem::path path = s_ShadersFolder / filename;
				result							 = document.load_file(path.generic_string().c_str());
			}
			else
			{
				return true;
			}
		}
		else
		{
			auto path = filename;
			path.replace_extension("xml");
			return LoadDoc(document, path);
		}

		if (result.status == pugi::status_ok)
			return true;
		return false;
	}
};

//=================================================================================
template <class StageHandle> const std::filesystem::path C_ShaderLoader<StageHandle>::s_ShadersFolder = "Shaders/";

//=================================================================================
// ShaderCompilerTrait
//=================================================================================
template <class StageHandle>
bool ShaderCompilerTrait<StageHandle>::compileShaderStage(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage)
{
	std::vector<char> src;
	if (!_loadFile(filepath, src))
	{
		CORE_LOG(E_Level::Error, E_Context::Render, "Failed to open shader file: {}", filepath.generic_string());
		return false;
	}
	m_TouchedFiles.emplace_back(filepath);

	return compileShaderStageInternal(stage, filepath, shaderStage, src, "main");
}

//=================================================================================
template <class StageHandle> void ShaderCompilerTrait<StageHandle>::Reset()
{
	m_TouchedFiles.clear();
}

//=================================================================================
template <class StageHandle> typename ShaderCompilerTrait<StageHandle>::T_Paths ShaderCompilerTrait<StageHandle>::GetTouchedFiles() const
{
	return m_TouchedFiles;
}

//=================================================================================
template <class StageHandle> bool ShaderCompilerTrait<StageHandle>::_loadFile(const std::filesystem::path& file, std::vector<char>& content)
{
	std::ifstream stream(file, std::ios::ate | std::ios::binary);

	if (stream.fail())
		return false;

	const std::size_t fileSize = (size_t)stream.tellg();
	content.resize(fileSize);
	stream.seekg(0);
	stream.read(content.data(), fileSize);

	stream.close();
	return true;
}

} // namespace GLEngine::Renderer
