#pragma once

#include <Renderer/Render/IndirectDraw.h>
#include <Renderer/Renderer2D.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>

#include <Physics/Quantities.h>

#include <Utils/HighResolutionTimer.h>
#include <Utils/Padding.h>

#include <rttr/registration_friend.h>

namespace GLEngine::GLRenderer {
class C_GLDevice;
class C_RenderInterface;
class C_GLRendererInterface2D;
class C_Framebuffer;

class C_RopeSimulation : public GUI::C_Window {
public:
	C_RopeSimulation(const GUID guid, GUI::C_GUIManager& guiMGR, C_GLDevice& device);

	void			   RequestDestroy() override;
	[[nodiscard]] bool CanDestroy() const override;
	void			   Update() override;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;

protected:
	void DrawComponents() const override;

private:
	struct Particle {
		Physics::Phys2D::position	  Position;
		Physics::Phys2D::position	  PrevPosition;
		Physics::Phys2D::acceleration Acceleration;
		int							  fixed = 0;
		::Utils::Padding<float, 1>	  gap;
	};

	void Setup();
	void Simulate();
	static void RelaxConstraint(Particle& p1, Particle& p2, Physics::Scalars::distance desiredDistance);

	std::unique_ptr<C_RenderInterface>		 m_RenderInterface;
	GUI::C_ImageViewer						 m_Image;
	std::unique_ptr<C_Framebuffer>			 m_FBO;
	Renderer::C_Renderer2D					 m_2DRenderer;
	std::unique_ptr<C_GLRendererInterface2D> m_2DRenderInterfaceHandles;
	Renderer::Handle<Renderer::Pipeline>	 m_Pipeline;
	Renderer::Handle<Renderer::Texture>		 m_DeviceImage;
	Renderer::Handle<Renderer::Texture>		 m_DeviceDepthImage;
	Renderer::Handle<Renderer::Texture>		 m_WorldOverlay;
	Renderer::C_TextureViewStorageCPU<float> m_OverlayStorage;
	Renderer::Handle<Renderer::Buffer>		 m_IndirectHandle;
	Renderer::Handle<Renderer::Buffer>		 m_ParticlesHandle;
	Renderer::IndirectDraw					 m_DrawCmd;

	std::vector<Particle> m_Particles;
	int					  m_NumParticles;
	mutable bool		  m_bScheduledSetup;

	bool								 m_bRunSimulation;
	mutable ::Utils::HighResolutionTimer m_Timer;
};
} // namespace GLEngine::GLRenderer
