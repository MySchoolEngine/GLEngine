#pragma once

#include <Editor/EditorApi.h>

#include <Renderer/RayCasting/RayRenderer.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <glm/glm.hpp>

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string_view>

// Forward declarations — ImageViewer.h requires imgui.h; include it only in .cpp files
namespace GLEngine::GUI {
class C_ImageViewer;
} // namespace GLEngine::GUI

namespace GLEngine::Renderer {
enum class E_TextureFormat : std::uint8_t;
} // namespace GLEngine::Renderer

namespace GLEngine::Editor {

enum class E_DebugTarget
{
	RowHeatMap,
	Normals,
	UV,
};

struct S_DebugBuffer {
	Renderer::Handle<Renderer::Texture>						m_GPUHandle;
	std::optional<Renderer::C_TextureViewStorageCPU<float>> m_Storage;
	std::unique_ptr<GUI::C_ImageViewer>						m_Viewer;

	S_DebugBuffer()							= default;
	S_DebugBuffer(const S_DebugBuffer&)		= delete;
	S_DebugBuffer(S_DebugBuffer&&) noexcept = default;
	S_DebugBuffer& operator=(const S_DebugBuffer&) = delete;
	S_DebugBuffer& operator=(S_DebugBuffer&&) noexcept = default;
};

/**
 * Shared render-thread state embedded in per-tab data for ray-tracing preview
 * editors (MaterialPreviewWindow, TrimeshPreviewWindow).
 *
 * Non-copyable and non-moveable (contains atomic and mutex members).
 * Always heap-allocate via unique_ptr inside the tab struct.
 */
struct S_RayPreviewState {
	Renderer::Handle<Renderer::Texture>						m_GPUImageHandle;
	std::optional<Renderer::C_TextureViewStorageCPU<float>> m_ImageStorage;
	std::optional<Renderer::C_TextureViewStorageCPU<float>> m_SamplesStorage;
	std::unique_ptr<GUI::C_ImageViewer>						m_GUIImage;
	std::unique_ptr<Renderer::C_RayRenderer>				m_Renderer;

	std::mutex		  m_ImageLock;
	std::atomic<int>  m_NumSamples{0};
	std::atomic<bool> m_Running{false};
	std::atomic<bool> m_StopRequested{false};

	// Sparse map of additional debug render targets (normals, UV, heat-map …).
	// Only registered entries are stored — no wasted allocations for unused slots.
	std::map<E_DebugTarget, S_DebugBuffer> m_DebugTargets;

	// Returns nullptr if the target was not registered via AddDebugTarget(s).
	[[nodiscard]] const S_DebugBuffer* GetDebugTarget(E_DebugTarget t) const;
	[[nodiscard]] S_DebugBuffer*	   GetDebugTarget(E_DebugTarget t);

	// Builds a C_RayRenderer::AdditionalTargets from whatever slots are registered.
	[[nodiscard]] Renderer::C_RayRenderer::AdditionalTargets BuildAdditionalTargets();

	S_RayPreviewState()								= default;
	S_RayPreviewState(const S_RayPreviewState&)		= delete;
	S_RayPreviewState(S_RayPreviewState&&) noexcept = delete;
	S_RayPreviewState& operator=(const S_RayPreviewState&) = delete;
	S_RayPreviewState& operator=(S_RayPreviewState&&) noexcept = delete;
};

// Allocates GPU texture + sampler, initialises image/samples storage, clears to black,
// uploads initial black frame, and creates the C_ImageViewer.
EDITOR_API_EXPORT void CreateRayPreviewState(S_RayPreviewState& state, glm::uvec2 resolution, std::string_view name, Renderer::E_TextureFormat format);

// Sets m_StopRequested, busy-waits until m_Running is false, then destroys the GPU texture.
EDITOR_API_EXPORT void DestroyRayPreviewState(S_RayPreviewState& state);

// Sets m_StopRequested and busy-waits until m_Running is false.
EDITOR_API_EXPORT void StopPreviewRender(S_RayPreviewState& state);

// try_lock + upload to GPU if the renderer has a new result pending.
EDITOR_API_EXPORT void UploadPreviewStorage(S_RayPreviewState& state);

// Draws ProgressBar + status text, or "Done" + Re-render button.
// Returns true when the Re-render button is clicked.
EDITOR_API_EXPORT bool DrawRenderProgress(const S_RayPreviewState& state, int targetSamples);

// Allocates GPU texture + CPU storage (and optionally a C_ImageViewer) for one debug slot.
// Safe to call multiple times with different targets.
EDITOR_API_EXPORT void
AddDebugTarget(S_RayPreviewState& state, E_DebugTarget target, glm::uvec2 resolution, std::string_view name, Renderer::E_TextureFormat format, bool createViewer);

// Convenience: register several targets at once with the same resolution/format/viewer setting.
// Each target's GPU texture is named "<namePrefix>_<TargetName>".
EDITOR_API_EXPORT void AddDebugTargets(S_RayPreviewState&			  state,
									   const std::set<E_DebugTarget>& targets,
									   glm::uvec2					  resolution,
									   std::string_view				  namePrefix,
									   Renderer::E_TextureFormat	  format,
									   bool							  createViewer);

} // namespace GLEngine::Editor
