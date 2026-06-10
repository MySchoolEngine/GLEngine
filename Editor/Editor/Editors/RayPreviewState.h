#pragma once

#include <Editor/EditorApi.h>

#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <glm/glm.hpp>

#include <atomic>
#include <memory>
#include <mutex>
#include <optional>
#include <string_view>

// Forward declarations — ImageViewer.h requires imgui.h; include it only in .cpp files
namespace GLEngine::GUI {
class C_ImageViewer;
} // namespace GLEngine::GUI

namespace GLEngine::Renderer {
enum class E_TextureFormat : std::uint8_t;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

/**
 * Shared render-thread state embedded in per-tab data for ray-tracing preview
 * editors (MaterialPreviewWindow, TrimeshPreviewWindow).
 *
 * Non-copyable and non-moveable (contains atomic and mutex members).
 * Always heap-allocate via unique_ptr inside the tab struct.
 */
struct S_RayPreviewState {
	Renderer::Handle<Renderer::Texture>						  m_GPUImageHandle;
	std::optional<Renderer::C_TextureViewStorageCPU<float>>   m_ImageStorage;
	std::optional<Renderer::C_TextureViewStorageCPU<float>>   m_SamplesStorage;
	std::unique_ptr<GUI::C_ImageViewer>						  m_GUIImage;
	std::unique_ptr<Renderer::C_RayRenderer>				  m_Renderer;

	std::mutex		  m_ImageLock;
	std::atomic<int>  m_NumSamples{0};
	std::atomic<bool> m_Running{false};
	std::atomic<bool> m_StopRequested{false};

	S_RayPreviewState()								= default;
	S_RayPreviewState(const S_RayPreviewState&)		= delete;
	S_RayPreviewState(S_RayPreviewState&&) noexcept = delete;
	S_RayPreviewState& operator=(const S_RayPreviewState&)		= delete;
	S_RayPreviewState& operator=(S_RayPreviewState&&) noexcept	= delete;
};

// Allocates GPU texture + sampler, initialises image/samples storage, clears to black,
// uploads initial black frame, and creates the C_ImageViewer.
EDITOR_API_EXPORT void CreateRayPreviewState(S_RayPreviewState& state,
											 glm::uvec2			resolution,
											 std::string_view	name,
											 Renderer::E_TextureFormat format);

// Sets m_StopRequested, busy-waits until m_Running is false, then destroys the GPU texture.
EDITOR_API_EXPORT void DestroyRayPreviewState(S_RayPreviewState& state);

// Sets m_StopRequested and busy-waits until m_Running is false.
EDITOR_API_EXPORT void StopPreviewRender(S_RayPreviewState& state);

// try_lock + upload to GPU if the renderer has a new result pending.
EDITOR_API_EXPORT void UploadPreviewStorage(S_RayPreviewState& state);

// Draws ProgressBar + status text, or "Done" + Re-render button.
// Returns true when the Re-render button is clicked.
EDITOR_API_EXPORT bool DrawRenderProgress(const S_RayPreviewState& state, int targetSamples);

} // namespace GLEngine::Editor
