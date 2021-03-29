#pragma once

#include <Renderer/ILight.h>
#include <Renderer/RendererApi.h>

#include <GUI/Input/Color.h>
#include <GUI/Input/Slider.h>

#include <Entity/ComponentBuilder.h>
#include <Entity/IComponent.h>

namespace GLEngine::Renderer {


//=============================================================
class RENDERER_API_EXPORT C_AreaLight : public I_Light {
public:
	explicit C_AreaLight(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_AreaLight();

	virtual void			 DebugDrawGUI(GUI::C_GUIManager* guiMGR = nullptr) override;
	virtual std::string_view GetDebugComponentName() const override;
	virtual bool			 HasDebugDrawGUI() const override;

	//================================================================
	// I_Light
	[[nodiscard]] virtual Physics::Primitives::C_Frustum GetShadingFrustum() const override;
	[[nodiscard]] float									 GetWidth() const { return m_WidthSlider.GetValue(); }
	[[nodiscard]] float									 GetHeight() const { return m_HeightSlider.GetValue(); }

	[[nodiscard]] glm::vec3 GetNormal() const;
	[[nodiscard]] glm::vec3 GetUpVector() const;

	virtual Physics::Primitives::S_AABB GetAABB() const override;

	virtual void DebugDraw() const {}

	[[nodiscard]] glm::vec3 DiffuseColour() const;
	[[nodiscard]] glm::vec3 SpecularColour() const;

protected:
	GUI::Input::C_Slider<float> m_WidthSlider, m_HeightSlider;
	GUI::Input::C_ColorRBG		m_DiffuseColor, m_SpecularColor;

	friend class C_AreaLightCompBuilder;
};

//=============================================================
class C_AreaLightCompBuilder : public Entity::I_ComponenetBuilder {
public:
	RENDERER_API_EXPORT [[nodiscard]] virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};

} // namespace GLEngine::Renderer
