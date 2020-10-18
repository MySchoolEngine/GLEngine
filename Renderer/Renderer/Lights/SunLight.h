#pragma once

#include <Renderer/ILight.h>
#include <Renderer/RendererApi.h>


namespace GLEngine::Renderer {

//=============================================================
class RENDERER_API_EXPORT C_SunLight : public I_Light
{
public:
	C_SunLight(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_SunLight();

	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override final;
	[[nodiscard]] glm::vec3 GetSunDirection() const;
	[[nodiscard]] glm::vec3 GetSunColor() const;
};
}
