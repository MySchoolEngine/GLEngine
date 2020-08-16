#pragma once

#include <Renderer/Lights/AreaLight.h>

#include <GLRenderer/GUI/Input/Vector.h>
#include <GLRenderer/GUI/Input/Slider.h>
#include <GLRenderer/GUI/Input/Color.h>

namespace GLEngine::GLRenderer {
namespace Textures
{
class C_Texture;
}

class C_GLAreaLight : public Renderer::C_AreaLight
{
public:
	explicit C_GLAreaLight(std::shared_ptr<Entity::I_Entity> owner);
	const std::shared_ptr<Textures::C_Texture>& GetShadowMap() const;

	//=================================================================================
	[[nodiscard]] virtual float GetWidth() const override;
	[[nodiscard]] virtual float GetHeight() const override;
	[[nodiscard]] virtual Physics::Primitives::C_Frustum GetShadingFrustum() const override;
	virtual void DebugDrawGUI() override;

	void DebugDraw() const;

	[[nodiscard]] glm::vec3 DiffuseColour() const;
	[[nodiscard]] glm::vec3 SpecularColour() const;


private:
	std::shared_ptr<Textures::C_Texture> m_ShadowMap;
	GUI::Input::C_Vec3 m_Pos;
	GUI::Input::C_Slider<float> m_WidthSlider, m_HeightSlider;
	GUI::Input::C_ColorRBG			m_DiffuseColor, m_SpecularColor;
};
}
