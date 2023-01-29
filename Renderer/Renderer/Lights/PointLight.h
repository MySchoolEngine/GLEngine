#pragma once

#include <Renderer/Colours.h>
#include <Renderer/ILight.h>
#include <Renderer/RendererApi.h>

#include <GUI/Input/Color.h>
#include <GUI/Input/Slider.h>

#include <Entity/IComponent.h>

#include <rttr/type>

namespace GLEngine::Renderer {

namespace MeshData {
struct Light;
}

//=============================================================
class RENDERER_API_EXPORT C_PointLight : public I_Light {
public:
	explicit C_PointLight(std::shared_ptr<Entity::I_Entity> owner);
	C_PointLight(std::shared_ptr<Entity::I_Entity> owner, const MeshData::Light& def);
	C_PointLight();
	virtual ~C_PointLight();

	[[nodiscard]] glm::vec3			GetPosition() const;
	[[nodiscard]] float				GetIntensity() const;
	[[nodiscard]] Colours::T_Colour GetColor() const;

	//================================================================
	// I_Light
	[[nodiscard]] Physics::Primitives::C_Frustum GetShadingFrustum() const override;

	[[nodiscard]] virtual std::string_view			  GetDebugComponentName() const override;
	[[nodiscard]] virtual bool						  HasDebugDrawGUI() const override;
	virtual void									  DebugDrawGUI() override;
	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override final;

private:
	float			  m_Intensity;
	Colours::T_Colour m_Color;

	RTTR_ENABLE(I_Light);
	RTTR_REGISTRATION_FRIEND;
};
} // namespace GLEngine::Renderer
