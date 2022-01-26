#pragma once

#include <Renderer/ILight.h>
#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>

namespace GLEngine::Renderer {
class I_DebugDraw;

//=============================================================
class RENDERER_API_EXPORT C_AreaLight : public I_Light {
public:
	explicit C_AreaLight(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~C_AreaLight();

	virtual void			 DebugDrawGUI() override;
	virtual std::string_view GetDebugComponentName() const override;
	virtual bool			 HasDebugDrawGUI() const override;

	//================================================================
	// I_Light
	[[nodiscard]] virtual Physics::Primitives::C_Frustum GetShadingFrustum() const override;
	[[nodiscard]] float									 GetWidth() const { return m_Width; }
	[[nodiscard]] float									 GetHeight() const { return m_Height; }

	[[nodiscard]] glm::vec3 GetNormal() const;
	[[nodiscard]] glm::vec3 GetUpVector() const;

	virtual Physics::Primitives::S_AABB GetAABB() const override;

	[[nodiscard]] Colours::T_Colour DiffuseColour() const;
	[[nodiscard]] Colours::T_Colour SpecularColour() const;

	void DebugDraw(I_DebugDraw* dd) const;

	RTTR_ENABLE(I_Light);
	RTTR_REGISTRATION_FRIEND;

protected:
	float						m_Width;
	float						m_Height;
	Colours::T_Colour			m_DiffuseColor;
	Colours::T_Colour			m_SpecularColor;

	friend class C_AreaLightCompBuilder;
};

//=============================================================
class C_AreaLightCompBuilder : public Entity::I_ComponenetBuilder {
public:
	RENDERER_API_EXPORT [[nodiscard]] virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};

} // namespace GLEngine::Renderer
