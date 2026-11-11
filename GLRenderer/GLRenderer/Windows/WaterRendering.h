#pragma once

#include <Renderer/Render/IndirectDraw.h>
#include <Renderer/Renderer2D.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>
#include <GUI/StrongTypeReflection.h>

#include <Physics/Quantities.h>

#include <Utils/HighResolutionTimer.h>
#include <Utils/Padding.h>

#include <rttr/registration_friend.h>

namespace GLEngine::GLRenderer {
class C_RenderInterface;
class C_GLRendererInterface2D;
class C_Framebuffer;

class C_WaterRendering : public GUI::C_Window {
public:
	C_WaterRendering(const GUID guid, GUI::C_GUIManager& guiMGR, C_GLDevice& device);

	void			   RequestDestroy() override;
	[[nodiscard]] bool CanDestroy() const override;
	void			   Update() override;

	RTTR_ENABLE();
	RTTR_REGISTRATION_FRIEND;

protected:
	void DrawComponents() const override;

private:
	struct Particle {
		Physics::Phys2D::position  Position;
		Physics::Phys2D::velocity  Velocity{0.f, 0.f};
		Physics::Scalars::density  LocalDensity{0.f};
		::Utils::Padding<float, 3> gap;

		void Move(const Physics::Scalars::time_type t) { Position += Velocity * t; }
	};

	void					  Simulate();
	void					  Collision(Particle& particle, const Physics::Scalars::time_type t);
	void					  Setup();
	Physics::Scalars::density GetLocalDensity(const Particle& samplingParticle) const;
	Physics::Scalars::density GetLocalDensity(const Physics::Phys2D::position& position) const;

	[[nodiscard]] static float			 SmoothingKernel(const float radius, Physics::Scalars::distance distance);
	[[nodiscard]] static float			 SmoothingKernelDerivative(const float radius, const Physics::Scalars::distance distance);
	[[nodiscard]] static float			 SharpSmoothingKernel(const float radius, const Physics::Scalars::distance distance);
	[[nodiscard]] static float			 SharpSmoothingKernelDerivative(const float radius, const Physics::Scalars::distance distance);
	[[nodiscard]] Physics::Phys2D::force CalculatePressureForce(const Physics::Phys2D::position& pos) const;
	float								 ConvertDensityToPressure(const Physics::Scalars::density density) const;

	std::unique_ptr<C_RenderInterface>		 m_RenderInterface;
	GUI::C_ImageViewer						 m_Image;
	std::unique_ptr<C_Framebuffer>			 m_FBO;
	Renderer::C_Renderer2D					 m_2DRenderer;
	std::unique_ptr<C_GLRendererInterface2D> m_2DRenderInterfaceHandles;
	Renderer::Handle<Renderer::Pipeline>	 m_Pipeline;
	Renderer::Handle<Renderer::Texture>		 m_DeviceImage;
	Renderer::Handle<Renderer::Texture>		 m_DeviceDepthImage;
	Renderer::Handle<Renderer::Buffer>		 m_IndirectHandle;
	Renderer::Handle<Renderer::Buffer>		 m_ParticlesHandle;
	std::vector<Renderer::IndirectDraw>		 m_IndirectData;
	Renderer::IndirectDraw					 m_DrawCmd;

	Renderer::Handle<Renderer::Texture>		 m_WorldOverlay;
	Renderer::C_TextureViewStorageCPU<float> m_OverlayStorage;

	std::vector<Particle>				 m_Particles;
	int									 m_NumParticles;
	float								 m_DensityRadius;
	float								 m_PressureMultiplier;
	float								 m_DensityDivisor;
	float								 m_ParticleRadius;
	float								 m_DesiredPressure;
	Physics::Scalars::mass				 m_ParticleMass;
	bool								 m_bRunSimulation;
	bool								 m_bGravitation;
	mutable bool						 m_bScheduledSetup;
	mutable ::Utils::HighResolutionTimer m_Timer;
};
} // namespace GLEngine::GLRenderer