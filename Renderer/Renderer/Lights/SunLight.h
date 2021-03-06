#pragma once

#include <Renderer/ILight.h>
#include <Renderer/RendererApi.h>

#include <GUI/Input/Color.h>
#include <GUI/Input/Slider.h>

namespace GLEngine::Renderer {
class I_DebugDraw;

//=============================================================
class RENDERER_API_EXPORT C_SunLight : public I_Light {
public:
	C_SunLight(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_SunLight();

	[[nodiscard]] glm::vec3			GetSunDirection() const;
	[[nodiscard]] Colours::T_Colour GetSunColor() const;
	[[nodiscard]] float				SunDiscMultiplier() const;
	[[nodiscard]] float				AtmosphereAsymetricFactor() const;

	[[nodiscard]] Physics::Primitives::C_Frustum GetShadingFrustum() const override;
	[[nodiscard]] bool							 HasDebugDrawGUI() const override;
	[[nodiscard]] std::string_view				 GetDebugComponentName() const override;
	virtual void								 DebugDrawGUI() override;

	void									  DebugDraw(I_DebugDraw* dd) const;
	[[nodiscard]] Physics::Primitives::S_AABB GetAABB() const override;

private:
	GUI::Input::C_Slider<float> m_SunX;
	GUI::Input::C_Slider<float> m_SunY;
	GUI::Input::C_Slider<float> m_SunZ;
	GUI::Input::C_ColorRBG		m_SunColor;
	GUI::Input::C_Slider<float> m_AsymetricFactor;
	GUI::Input::C_Slider<float> m_SunDiscMultiplier;
};
} // namespace GLEngine::Renderer
