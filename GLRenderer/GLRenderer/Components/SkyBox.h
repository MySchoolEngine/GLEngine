#pragma once

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/VAO/VAO.h>

#include <Renderer/IRenderableComponent.h>

namespace GLEngine::GLRenderer::Components {

class C_SkyBox : public Renderer::I_RenderableComponent {
public:
	enum class E_Side
	{
		Right	= 0,
		Left	= 1,
		Top		= 2,
		Bottom	= 3,
		Back	= 4,
		Forward = 5,
	};
	explicit C_SkyBox(std::shared_ptr<Entity::I_Entity> owner);
	void AddTexture(E_Side side, const std::filesystem::path& filename);

	//==========================================
	// Renderer::I_RenderableComponent
	//==========================================
	virtual void PerformDraw() const override;

	[[nodiscard]] virtual Physics::Primitives::S_AABB GetAABB() const override;


	virtual std::string_view GetDebugComponentName() const override;
	virtual bool			 HasDebugDrawGUI() const override;

private:
	//Textures::C_Texture							 m_Textures;
	VAO::C_GLVAO<1>								 m_VAO;
	static constexpr Physics::Primitives::S_AABB m_AABB = Physics::Primitives::S_AABB();
};
} // namespace GLEngine::GLRenderer::Components