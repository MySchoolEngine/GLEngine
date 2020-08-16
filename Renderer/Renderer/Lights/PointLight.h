#pragma once

#include <Renderer/ILight.h>
#include <Renderer/RendererApi.h>

#include <Renderer/Mesh/Scene.h>

#include <Entity/IComponent.h>

namespace GLEngine::Renderer {

//=============================================================
class RENDERER_API_EXPORT I_PointLight : public I_Light
{
public:
	I_PointLight(std::shared_ptr<Entity::I_Entity> owner);
	virtual ~I_PointLight();
	virtual glm::vec3 GetPosition() const = 0;
	virtual float GetIntensity() const = 0;
	virtual glm::vec3 GetColor() const = 0;
};

//=============================================================
class RENDERER_API_EXPORT C_PointLight : public I_PointLight {
public:
	explicit C_PointLight(std::shared_ptr<Entity::I_Entity> owner);
	C_PointLight(std::shared_ptr<Entity::I_Entity> owner, const MeshData::Light& def);
	virtual ~C_PointLight();

	[[nodiscard]] virtual glm::vec3 GetPosition() const override;
	[[nodiscard]] virtual float GetIntensity() const override;
	[[nodiscard]] virtual glm::vec3 GetColor() const override;



	//================================================================
	// I_Light
	[[nodiscard]] Physics::Primitives::C_Frustum GetShadingFrustum() const override;

private:
#pragma warning(push)
#pragma warning( disable : 4251)
	float		m_Intensity;
	glm::vec3	m_Color;
	glm::vec3	m_Offset;
#pragma warning(pop)

	friend class C_PointLightCompBuilder;
};

//=============================================================
class C_PointLightCompBuilder : public Entity::I_ComponenetBuilder
{
public:
	RENDERER_API_EXPORT virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override;
};
}
