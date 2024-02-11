#include <Utils/STLAfx.h>

#include <Renderer/Shaders/ShaderPreprocessor.h>
#include <Renderer/Shaders/ShaderCompiling.h>

#include <GLRenderer/Shaders/Generation/ShaderTypesReflection.h>

#include <Utils/Logging/Logging.h>
#include <Utils/Logging/ILogger.h>

#include <iostream>
#include <fstream>

class C_ShaderCompiler : public GLEngine::Renderer::ShaderCompilerTrait<std::string>
{
	using T_Base = GLEngine::Renderer::ShaderCompilerTrait<std::string>;
public:
	C_ShaderCompiler()
		: T_Base(std::ios::binary)
	{}
	void ReleaseStage(T_StageHandle& stage) override
	{
		// nothing
	}
protected:
	bool compileShaderStageInternal(T_StageHandle& stage, const std::filesystem::path& filepath, const GLEngine::Renderer::E_ShaderStage shaderStage, std::vector<char>& content, const std::string& entryPoint) override
	{
		GLEngine::Renderer::Shaders::C_ShaderPreprocessor preproces(std::make_unique<GLEngine::GLRenderer::Shaders::C_GLCodeProvider>());
		preproces.Define("VULKAN", "1");
		std::string strContent(content.begin(), content.end());
		stage = preproces.PreprocessFile(strContent, filepath.parent_path());

		return preproces.WasSuccessful();
	}
};

static const std::filesystem::path gs_OutputFolder = "obj/shaders/compile";
static const std::filesystem::path gs_GLSLC = VULKAN_GLSLC;

bool PrepareFolder(const std::filesystem::path& path)
{
	const auto debugDirectory = path.parent_path();
	if (!std::filesystem::exists(debugDirectory))
	{
		if (!std::filesystem::create_directories(debugDirectory))
		{
			CORE_LOG(E_Level::Error, E_Context::Render, "Cannot create debug output directory");
			return false;
		}
	}
	return false;
}

int main(int argc, char** argv)
{
	using namespace GLEngine::Renderer;
	C_ShaderCompiler compiler;
	C_ShaderLoader loader(compiler);

	if (argc == 2 && std::string_view(argv[1]) == "-h")
	{
		CORE_LOG(E_Level::Info, E_Context::Core, "Help page for ShaderProcessor.exe");
		CORE_LOG(E_Level::Info, E_Context::Core, "-s [shaderName] to compile given shader");
		CORE_LOG(E_Level::Info, E_Context::Core, "Shader processor will output you file into workFolder/obj/shaders/compile/*.glsl");

		return 0;
	}
	else if (argc >= 3 && std::string_view(argv[1]) == "-s")
	{
		if (argc == 4 && std::string_view(argv[3]) == "--silent")
		{
		}
		else
		{
			auto& logging = Utils::Logging::C_LoggingSystem::Instance();
			logging.AddLogger(new Utils::Logging::C_CoutLogger());
		}
		PrepareFolder(gs_OutputFolder);
		std::ofstream batfile(gs_OutputFolder / "compile.bat");

		std::vector<C_ShaderLoader<std::string>::T_ShaderStage> stages;
		if (!loader.LoadAllStages(argv[2], stages))
		{
			std::cerr << "Error while loading files!\n";
			return 1;
		}
		for (const auto& stage : stages)
		{
			std::filesystem::path filename = "";
			std::string stageArgument;
			switch (stage.first)
			{
			case E_ShaderStage::Vertex:
				filename = "Vertex.glsl";
				stageArgument = "vertex";
				break;
			case E_ShaderStage::Fragment:
				filename = "Fragment.glsl";
				stageArgument = "fragment";
				break;
			case E_ShaderStage::Geometry:
				filename = "Geometry.glsl";
				stageArgument = "geometry";
				break;
			case E_ShaderStage::TesselationControl:
				filename = "TesselationControl.glsl";
				stageArgument = "tesscontrol";
				break;
			case E_ShaderStage::TesselationEvaluation:
				filename = "TesselationEvaluation.glsl";
				stageArgument = "tesseval";
				break;
			case E_ShaderStage::Compute:
				filename = "Compute.glsl";
				stageArgument = "compute";
				break;
			case E_ShaderStage::RayGen:
				filename = "RayGen.glsl";
				break;
			}

			const auto fullPath = gs_OutputFolder / argv[2] / filename;

			std::ofstream intermediateFile;

			PrepareFolder(fullPath);

			intermediateFile.open(fullPath);
			if (!intermediateFile.is_open())
			{
				CORE_LOG(E_Level::Error, E_Context::Render, "Cannot open file for debug output");
			}
			intermediateFile << stage.second;
			intermediateFile.close();

			const std::filesystem::path relativeFolder(argv[2]);
			auto outputFilename = filename;
			outputFilename.replace_extension("spv");

			batfile << gs_GLSLC.generic_string()
				<< " -fshader-stage=" << stageArgument
				<< " " << (relativeFolder / filename).generic_string()
				<< " -o " << (relativeFolder / outputFilename).generic_string() << "\n";
		}

		batfile << "pause\n";
		batfile.close();
		CORE_LOG(E_Level::Info, E_Context::Core, "Successfully created bat file");
	}
	else
	{
		std::cerr << "Unknown arguments!\n";
		return 1;
	}

	return 0;
}