#pragma once

#include <Renderer/Lights/AreaLight.h>

#include <GLRenderer/GUI/Input/Vector.h>

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
	Physics::Primitives::C_Frustum GetShadingFrustum() const override;
	virtual void DebugDrawGUI() override;

	void DebugDraw() const;

private:
	std::shared_ptr<Textures::C_Texture> m_ShadowMap;
	GUI::Input::C_Vec3 m_Pos;
};
}
