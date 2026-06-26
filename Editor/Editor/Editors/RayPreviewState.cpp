#include <EditorStdafx.h>

#include <Editor/Editors/RayPreviewState.h>

#include <Renderer/ICameraComponent.h>
#include <Renderer/IRenderer.h>
#include <Renderer/RayCasting/RayGeneration/InterleavedLinesFactory.h>
#include <Renderer/Resources/ResourceManager.h>
#include <Renderer/Textures/TextureView.h>

#include <GUI/ImageViewer.h>

#include <Core/Application.h>

#include <Utils/HighResolutionTimer.h>

#include <chrono>
#include <imgui.h>
#include <thread>

namespace GLEngine::Editor {

namespace {
//=================================================================================
std::string DebugTargetSuffix(E_DebugTarget t)
{
	switch (t)
	{
	case E_DebugTarget::RowHeatMap:
		return "_RowHeatMap";
	case E_DebugTarget::Normals:
		return "_Normals";
	case E_DebugTarget::UV:
		return "_UV";
	}
	return "_Unknown";
}
} // namespace

//=================================================================================
void CreateRayPreviewState(S_RayPreviewState& state, glm::uvec2 resolution, std::string_view name, Renderer::E_TextureFormat format)
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	state.m_GPUImageHandle	 = renderer.GetRM().createTexture(Renderer::TextureDescriptor{
		  .name			 = std::string(name),
		  .width		 = resolution.x,
		  .height		 = resolution.y,
		  .type			 = Renderer::E_TextureType::TEXTURE_2D,
		  .format		 = format,
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
const S_DebugBuffer* S_RayPreviewState::GetDebugTarget(E_DebugTarget t) const
{
	const auto it = m_DebugTargets.find(t);
	return it != m_DebugTargets.end() ? &it->second : nullptr;
}

//=================================================================================
S_DebugBuffer* S_RayPreviewState::GetDebugTarget(E_DebugTarget t)
{
	const auto it = m_DebugTargets.find(t);
	return it != m_DebugTargets.end() ? &it->second : nullptr;
}

//=================================================================================
Renderer::C_RayRenderer::AdditionalTargets S_RayPreviewState::BuildAdditionalTargets()
{
	Renderer::C_RayRenderer::AdditionalTargets out{};
	auto									   get = [&](E_DebugTarget t) -> Renderer::I_TextureViewStorage* {
		  const auto it = m_DebugTargets.find(t);
		  return (it != m_DebugTargets.end() && it->second.m_Storage) ? &(it->second.m_Storage.value()) : nullptr;
	};
	out.rowHeatMap = get(E_DebugTarget::RowHeatMap);
	out.normalsMap = get(E_DebugTarget::Normals);
	out.uvMap	   = get(E_DebugTarget::UV);
	return out;
}

//=================================================================================
void AddDebugTarget(S_RayPreviewState& state, E_DebugTarget target, glm::uvec2 resolution, std::string_view name, Renderer::E_TextureFormat format, bool createViewer)
{
	auto& renderer = Core::C_Application::Get().GetActiveRenderer();

	const auto samplerHandle = renderer.GetRM().createSampler(Renderer::SamplerDescriptor2D{
		.m_FilterMin = Renderer::E_TextureFilter::Linear,
		.m_FilterMag = Renderer::E_TextureFilter::Linear,
		.m_WrapS	 = Renderer::E_WrapFunction::Repeat,
		.m_WrapT	 = Renderer::E_WrapFunction::Repeat,
		.m_WrapU	 = Renderer::E_WrapFunction::Repeat,
	});

	auto& slot		 = state.m_DebugTargets[target];
	slot.m_GPUHandle = renderer.GetRM().createTexture(Renderer::TextureDescriptor{
		.name		   = std::string(name),
		.width		   = resolution.x,
		.height		   = resolution.y,
		.type		   = Renderer::E_TextureType::TEXTURE_2D,
		.format		   = format,
		.m_bStreamable = false,
	});
	renderer.SetTextureSampler(slot.m_GPUHandle, samplerHandle);

	slot.m_Storage.emplace(resolution.x, resolution.y, 3);
	Renderer::C_TextureView(&*slot.m_Storage).ClearColor({0.f, 0.f, 0.f, 1.f});
	renderer.SetTextureData(slot.m_GPUHandle, *slot.m_Storage);

	if (createViewer)
	{
		slot.m_Viewer = std::make_unique<GUI::C_ImageViewer>(slot.m_GPUHandle);
		slot.m_Viewer->SetSize({resolution.x, resolution.y});
	}
}

//=================================================================================
void AddDebugTargets(S_RayPreviewState&				state,
					 const std::set<E_DebugTarget>& targets,
					 glm::uvec2						resolution,
					 std::string_view				namePrefix,
					 Renderer::E_TextureFormat		format,
					 bool							createViewer)
{
	for (const auto t : targets)
		AddDebugTarget(state, t, resolution, std::string(namePrefix) + DebugTargetSuffix(t), format, createViewer);
}

//=================================================================================
void StartPreviewRender(S_RayPreviewState& state, Renderer::I_CameraComponent& camera, int targetSamples)
{
	if (state.m_Running.load())
		return;

	state.m_NumSamples.store(0);
	state.m_StopRequested.store(false);
	state.m_Running.store(true);

	std::thread([&state, &camera, targetSamples]() {
		while (!state.m_StopRequested.load())
		{
			const int samplesBefore = state.m_NumSamples.load();
			if (samplesBefore >= targetSamples)
				break;

			::Utils::HighResolutionTimer timer;
			state.m_Renderer->Render(camera, *state.m_ImageStorage, *state.m_SamplesStorage, &state.m_ImageLock, samplesBefore,
									 Renderer::C_InterleavedLinesFactory{4}, state.BuildAdditionalTargets());
			state.m_NumSamples.fetch_add(1);
			CORE_LOG(E_Level::Info, E_Context::Render, "One iteration took {}s", static_cast<float>(timer.getElapsedTimeFromLastQueryMilliseconds()) / 1000.f);
		}
		state.m_Running.store(false);
	}).detach();
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
	for (auto& [key, slot] : state.m_DebugTargets)
	{
		if (slot.m_GPUHandle.IsValid())
			rm.destoryTexture(slot.m_GPUHandle);
	}
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
			auto& renderer = Core::C_Application::Get().GetActiveRenderer();
			renderer.SetTextureData(state.m_GPUImageHandle, *state.m_ImageStorage);
			for (auto& [key, slot] : state.m_DebugTargets)
			{
				if (slot.m_Storage && slot.m_GPUHandle.IsValid())
					renderer.SetTextureData(slot.m_GPUHandle, *slot.m_Storage);
			}
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
