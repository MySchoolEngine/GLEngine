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

#include <Renderer/Shaders/ShaderCompiling.h>

namespace GLEngine::GLRenderer::Shaders {

class C_ShaderCompiler : public Renderer::ShaderCompilerTrait<GLuint>
{
public:
	C_ShaderCompiler(bool preprocessorOutput = false);
	bool linkProgram(GLuint& program, const std::vector<std::pair<Renderer::E_ShaderStage, GLuint>>& stages);

	bool		m_PreprocessorOutput;
protected:
	virtual bool compileShaderStageInternal(T_StageHandle& stage, const std::filesystem::path& filepath, const Renderer::E_ShaderStage shaderStage, std::string& content) override;
};
}