#pragma once
#include <Renderer/Renderer2D.h>
#include <Renderer/Resources/RenderResourceHandle.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::GLRenderer {
class C_GLRendererInterface2D;
class C_Framebuffer;
class C_WaterRendering : public GUI::C_Window {
public:
	C_WaterRendering(GUID guid, GUI::C_GUIManager& guiMGR, C_GLDevice& device);

	void			   RequestDestroy() override;
	[[nodiscard]] bool CanDestroy() const override;
	void			   Update() override;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;

protected:
	void DrawComponents() const override;

private:
	void Simulate();
	void Setup();

	GUI::C_ImageViewer						 m_Image;
	std::unique_ptr<C_Framebuffer>			 m_FBO;
	Renderer::C_Renderer2D					 m_2DRenderer;
	std::unique_ptr<C_GLRendererInterface2D> m_2DRenderInterfaceHandles;
	Renderer::Handle<Renderer::Pipeline>	 m_Pipeline;
	Renderer::Handle<Renderer::Texture>		 m_DeviceImage;
	Renderer::Handle<Renderer::Texture>		 m_DeviceDepthImage;

	std::vector<glm::vec2> m_Particles;
	int					   m_NumParticles;
	bool				   m_bRunSimulation;
	mutable bool		   m_bScheduledSetup;
};
} // namespace GLEngine::GLRenderer