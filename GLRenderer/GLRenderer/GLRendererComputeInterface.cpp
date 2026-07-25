#include <GLRendererStdafx.h>

#include <GLRenderer/Commands/HACK/LambdaCommand.h>
#include <GLRenderer/GLRendererComputeInterface.h>
#include <GLRenderer/OGLRenderer.h>
#include <GLRenderer/Shaders/ShaderManager.h>
#include <GLRenderer/Textures/TextureUnitManager.h>

#include <Core/Application.h>

namespace GLEngine::GLRenderer {
void C_GLRendererComputeInterface::Render(const Renderer::ComputeRenderCall& call)
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();
	auto& glRM	   = dynamic_cast<C_OGLRenderer&>(renderer).GetRMGR(); // TODO make it DI

	const auto& pipeline = glRM.GetComputePipeline(call.PipelineHandle);

	auto& shmgr		  = Shaders::C_ShaderManager::Instance();
	auto  noiseShader = shmgr.GetProgram(pipeline.shader);
	if (!noiseShader)
		return;
	shmgr.ActivateShader(noiseShader);


	constexpr int	dim	   = 1024; // todo
	constexpr int	freq   = 24;
	constexpr float weight = 0.5f;

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[this, noiseShader]() {
			noiseShader->SetUniform("frequency", freq);
			noiseShader->SetUniform("unicoord", (glm::ivec2{0, 0} * (dim - 1)));
			noiseShader->SetUniform("patchWidth", dim);
			noiseShader->SetUniform("usePerlin", true);
			noiseShader->SetUniform("layerWeight[0]", weight);
			noiseShader->SetUniform("layerWeight[1]", weight);
			noiseShader->SetUniform("layerWeight[2]", weight);
		},
		"Prepare generation of noise"));


	RenderDoc::C_DebugScope s("NoiseCompute");
	auto&					tm = Textures::C_TextureUnitManger::Instance();
	for (int i = 0; i < call.Textures.size(); ++i)
	{
		auto* texture = glRM.GetTexture(call.Textures[i]);
		if (!texture)
			continue;

		tm.BindImageToUnit(*texture, i, E_OpenGLAccess::Write);
	}

	Core::C_Application::Get().GetActiveRenderer().AddCommand(std::make_unique<Commands::HACK::C_LambdaCommand>(
		[&]() {
			glDispatchCompute(call.NumWorkgroups.x, call.NumWorkgroups.y, call.NumWorkgroups.z);
			glMemoryBarrier(GL_ALL_BARRIER_BITS);

			// m_Noise.GenerateMipMaps();
		},
		"Dispatch noise"));
}
} // namespace GLEngine::GLRenderer