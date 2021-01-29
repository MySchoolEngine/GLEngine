#pragma once

#include <Renderer/RayCasting/RayTraceScene.h>
#include <Renderer/RendererApi.h>
#include <Renderer/Textures/TextureView.h>

namespace GLEngine::Renderer {
class I_CameraComponent;
class I_TextureViewStorage;
class RENDERER_API_EXPORT C_RayRenderer {
public:
	C_RayRenderer(const C_RayTraceScene& scene);

	void SetDirectionalDebug(C_OctahedralTextureView view) { m_DirectionsView.emplace(view); }

	void Render(I_CameraComponent& camera, I_TextureViewStorage& storage);

private:
	const C_RayTraceScene&				   m_Scene;
	std::optional<C_OctahedralTextureView> m_DirectionsView;
};
} // namespace GLEngine::Renderer
