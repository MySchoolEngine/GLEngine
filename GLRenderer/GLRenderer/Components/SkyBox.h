#pragma once

#include <Renderer/IRenderableComponent.h>

#include <GLRenderer/Textures/Texture.h>

namespace GLEngine {
namespace GLRenderer {
namespace Components {

class C_SkyBox : public Renderer::I_RenderableComponent {
public:
	enum class E_Side {
		Top = 0,
		Bottom = 1,
		Left = 2,
		Right = 3,
		Forward = 4,
		Back = 5,
	};
	C_SkyBox();
	void AddTexture(E_Side side, std::string& filename);

	//==========================================
	// Renderer::I_RenderableComponent
	//==========================================
	virtual void PerformDraw() const override;

private:
	Textures::C_Texture m_Textures;
};
}}}