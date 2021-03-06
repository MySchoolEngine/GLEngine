#pragma once

#include <Renderer/Colours.h>
#include <Renderer/ILight.h>
#include <Renderer/Mesh/Scene.h>
#include <Renderer/RendererApi.h>

#include <GUI/Input/Color.h>
#include <GUI/Input/Slider.h>

#include <Entity/IComponent.h>

namespace GLEngine::Renderer {

namespace MeshData {
struct Light;
}

//=============================================================
class RENDERER_API_EXPORT C_PointLight : public I_Light {
public:
	explicit C_PointLight(std::shared_ptr<Entity::I_Entity> owner);
	C_PointLight(std::shared_ptr<Entity::I_Entity> owner, const MeshData::Light& def);
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
	GUI::Input::C_Slider<float> m_Intensity;
	GUI::Input::C_ColorRBG		m_Color;

	friend class C_PointLightCompBuilder;
};

//=============================================================
class C_PointLightCompBuilder : public Entity::I_ComponenetBuilder {
public:
	RENDERER_API_EXPORT virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};
} // namespace GLEngine::Renderer
