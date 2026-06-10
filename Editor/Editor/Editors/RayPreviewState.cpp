#include <EditorStdafx.h>

#include <Editor/Editors/RayPreviewState.h>

#include <imgui.h>

#include <GUI/ImageViewer.h>

#include <Renderer/IRenderer.h>
#include <Renderer/Textures/TextureView.h>
#include <Renderer/Resources/ResourceManager.h>

#include <Core/Application.h>

#include <chrono>
#include <thread>

namespace GLEngine::Editor {

//=================================================================================
void CreateRayPreviewState(S_RayPreviewState& state,
						   glm::uvec2		  resolution,
						   std::string_view	  name,
						   Renderer::E_TextureFormat format)
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	state.m_GPUImageHandle = renderer.GetRM().createTexture(Renderer::TextureDescriptor{
		.name		   = std::string(name),
		.width		   = resolution.x,
		.height		   = resolution.y,
		.type		   = Renderer::E_TextureType::TEXTURE_2D,
		.format		   = format,
		.m_bStreamable = false,
	});
	const auto samplerHandle = renderer.GetRM().createSampler(Renderer::SamplerDescriptor2D{
		.m_FilterMin = Renderer::E_TextureFilter::Linear,
		.m_FilterMag = Renderer::E_TextureFilter::Linear,
		.m_WrapS	 = Renderer::E_WrapFunction::Repeat,
		.m_WrapT	 = Renderer::E_WrapFunction::Repeat,
		.m_WrapU	 = Renderer::E_WrapFunction::Repeat,
	});
	renderer.SetTextureSampler(state.m_GPUImageHandle, samplerHandle);

	state.m_ImageStorage.emplace(resolution.x, resolution.y, 3);
	state.m_SamplesStorage.emplace(resolution.x, resolution.y, 3);

	constexpr glm::vec4 black{0.f, 0.f, 0.f, 1.f};
	Renderer::C_TextureView(&*state.m_ImageStorage).ClearColor(black);
	Renderer::C_TextureView(&*state.m_SamplesStorage).ClearColor(black);
	renderer.SetTextureData(state.m_GPUImageHandle, *state.m_ImageStorage);

	state.m_GUIImage = std::make_unique<GUI::C_ImageViewer>(state.m_GPUImageHandle);
	state.m_GUIImage->SetSize({resolution.x, resolution.y});
}

//=================================================================================
void StopPreviewRender(S_RayPreviewState& state)
{
	state.m_StopRequested.store(true);
	while (state.m_Running.load())
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
}

//=================================================================================
void DestroyRayPreviewState(S_RayPreviewState& state)
{
	StopPreviewRender(state);
	auto& rm = Core::C_Application::Get().GetActiveRenderer().GetRM();
	if (state.m_GPUImageHandle.IsValid())
		rm.destoryTexture(state.m_GPUImageHandle);
}

//=================================================================================
void UploadPreviewStorage(S_RayPreviewState& state)
{
	if (!state.m_Renderer)
		return;

	if (state.m_ImageLock.try_lock())
	{
		if (state.m_Renderer->NewResultAvailable())
		{
			Core::C_Application::Get().GetActiveRenderer()
				.SetTextureData(state.m_GPUImageHandle, *state.m_ImageStorage);
			state.m_Renderer->SetResultConsumed();
		}
		state.m_ImageLock.unlock();
	}
}

//=================================================================================
bool DrawRenderProgress(const S_RayPreviewState& state, int targetSamples)
{
	const int samples = state.m_NumSamples.load();
	if (state.m_Running.load())
	{
		ImGui::ProgressBar(static_cast<float>(samples) / targetSamples, ImVec2(-1.f, 0.f));
		ImGui::Text("Rendering... %d / %d samples", samples, targetSamples);
		return false;
	}
	ImGui::Text("Done - %d samples", samples);
	return ImGui::Button("Re-render");
}

} // namespace GLEngine::Editor
