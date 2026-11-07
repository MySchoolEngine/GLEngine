#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Components/ForceLinkComponentTU.h>

#include <memory>

namespace GLEngine::Renderer {

class I_Camera;

// Not RTTR-registered and not serializable - cameras are created programmatically by
// AddMandatoryWorldParts, not hand-added in the Editor (userAddable=false below).
struct S_CameraComponent {
	std::shared_ptr<I_Camera> camera;
};

DECLARE_FORCE_LINK_COMPONENT_TU(EnsureCameraComponentsRegistered);

} // namespace GLEngine::Renderer
