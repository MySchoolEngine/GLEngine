#pragma once

#include <Renderer/RendererApi.h>

#include <memory>

namespace GLEngine::Renderer {

class I_Camera;

// Not RTTR-registered and not serializable - cameras are created programmatically by
// AddMandatoryWorldParts, not hand-added in the Editor (userAddable=false below).
struct S_CameraComponent {
	std::shared_ptr<I_Camera> camera;
};

// See LightComponents.h's EnsureLightComponentsRegistered for why this exists (force-link for
// static-lib test builds).
RENDERER_API_EXPORT void EnsureCameraComponentsRegistered();

} // namespace GLEngine::Renderer
