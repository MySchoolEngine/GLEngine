#include <RendererStdafx.h>

#include <Renderer/Components/Mesh/StaticMeshComponents.h>

#include <Entity/ComponentRegistry.h>

#include <Utils/Reflection/Metadata.h>
#include <Utils/Serialization/SerializationUtils.h>

#include <rttr/registration>

// clang-format off
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<S_StaticMesh>("StaticMesh")
		.constructor<>()(rttr::policy::ctor::as_object)
		.property("ModelRes", &S_StaticMesh::requested)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetamember<SerializationCls::MandatoryProperty>(true),
			RegisterMetamember<UI::MeshResource::Name>("Model"),
			REGISTER_DEFAULT_VALUE(GLEngine::Core::ResourceHandle<MeshResource>())
		)
		.property("Materials", &S_StaticMesh::materials)
		(
			rttr::policy::prop::as_reference_wrapper,
			RegisterMetamember<SerializationCls::MandatoryProperty>(true),
			RegisterMetamember<UI::MaterialResource::Name>("Material")
		);
}
// clang-format on

namespace {
[[maybe_unused]] const bool g_RegisterStaticMeshComponent = [] {
	GLEngine::Entity::C_ComponentRegistry::Instance().Register<GLEngine::Renderer::S_StaticMesh>("StaticMesh");
	return true;
}();
} // namespace

namespace GLEngine::Renderer {
DEFINE_FORCE_LINK_COMPONENT_TU(EnsureStaticMeshComponentsRegistered);
} // namespace GLEngine::Renderer
