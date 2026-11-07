#include <RendererStdafx.h>

#include <Renderer/Components/CameraComponent.h>

#include <Entity/ComponentRegistry.h>

namespace {
// Not serializable/user-addable/drawGUI-capable through the generic component registry path:
// cameras are created programmatically (AddMandatoryWorldParts) and keep their own, separate
// RTTR-driven XML serialization (the concrete I_Camera subclass, e.g. C_OrbitalCamera, is the
// node that gets saved/loaded, not S_CameraComponent itself).
[[maybe_unused]] const bool g_RegisterCameraComponent = [] {
	GLEngine::Entity::C_ComponentRegistry::Instance().Register<GLEngine::Renderer::S_CameraComponent>("Camera", /*serializable=*/false, /*userAddable=*/false, /*drawGUI=*/false);
	return true;
}();
} // namespace

namespace GLEngine::Renderer {
DEFINE_FORCE_LINK_COMPONENT_TU(EnsureCameraComponentsRegistered);
} // namespace GLEngine::Renderer
