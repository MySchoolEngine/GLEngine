#include <RendererStdafx.h>

#include <Renderer/Components/Mesh/GeomComponents.h>
#include <Renderer/Materials/Material.h>
#include <Renderer/Materials/MaterialManager.h>

#include <Entity/ComponentRegistry.h>

#include <rttr/registration>

namespace {
// Ported from C_GeomComponent::AfterDeserialize - re-registers the loaded material with the
// manager (the manager is the source of truth for "which materials currently exist").
void AfterDeserialize(GLEngine::Renderer::S_GeomComponent& component, GLEngine::Utils::DeserializeCtx&)
{
	GLEngine::Renderer::C_MaterialManager::Instance().RegisterMaterial(component.material);
}
} // namespace

RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;

	rttr::registration::class_<S_GeomComponent>("GeomComponent")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("Material", &S_GeomComponent::material)
		.method("AfterDeserialize", &AfterDeserialize)();
}

namespace {
const bool g_RegisterGeomComponent = [] {
	GLEngine::Entity::C_ComponentRegistry::Instance().Register<GLEngine::Renderer::S_GeomComponent>("Geometry");
	return true;
}();
} // namespace

namespace GLEngine::Renderer {
void EnsureGeomComponentsRegistered()
{
}
} // namespace GLEngine::Renderer
