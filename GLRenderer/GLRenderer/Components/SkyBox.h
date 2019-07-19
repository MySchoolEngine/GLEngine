#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Textures/Texture.h>
#include <GLRenderer/VAO/VAO.h>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

class C_SkyBox : public Renderer::I_RenderableComponent {
public:
	enum class E_Side {
		Right = 0,
		Left = 1,
		Top = 2,
		Bottom = 3,
		Back = 4,
		Forward = 5,
	};
	C_SkyBox();
	void AddTexture(E_Side side, std::string& filename);

	//==========================================
	// Renderer::I_RenderableComponent
	//==========================================
	virtual void PerformDraw() const override;

private:
	Textures::C_Texture m_Textures;
	VAO::C_GLVAO<1>		m_VAO;
};
}}}