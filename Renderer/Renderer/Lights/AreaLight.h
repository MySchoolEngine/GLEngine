#pragma once

#include <Renderer/ILight.h>
#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>

namespace GLEngine::Renderer {


//=============================================================
class RENDERER_API_EXPORT C_AreaLight : public I_Light {
public:
	C_AreaLight(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_AreaLight();


	//================================================================
	// I_Light
	[[nodiscard]] virtual Physics::Primitives::C_Frustum GetShadingFrustum() const override;
	[[nodiscard]] virtual float GetWidth() const { return m_Width; }
	[[nodiscard]] virtual float GetHeight() const { return m_Height; }

	virtual Physics::Primitives::S_AABB GetAABB() const override;

protected:
#pragma warning(push)
#pragma warning(disable:4251)
	glm::vec3 m_Position;
	glm::vec3 m_Normal;
	glm::vec3 m_UpVector;
#pragma warning(pop)
	float			m_Width;
	float			m_Height;
};

}

