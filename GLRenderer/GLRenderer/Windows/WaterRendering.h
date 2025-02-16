#pragma once

#include <Renderer/Render/IndirectDraw.h>
#include <Renderer/Renderer2D.h>
#include <Renderer/Resources/RenderResourceHandle.h>
#include <Renderer/Textures/Storage/TextureLinearStorage.h>

#include <GUI/GUIWindow.h>
#include <GUI/ImageViewer.h>

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
		glm::vec2				   Position;
		glm::vec2				   Velocity{0.f, 0.f};
		float					   LocalDensity = 0.f;
		::Utils::Padding<float, 3> gap;

		void Move(const float t) { Position += Velocity * t; }
	};

	void  Simulate();
	void  Collision(Particle& particle, const float t);
	void  Setup();
	float GetLocalDensity(const Particle& samplingParticle) const;
	float GetLocalDensity(const glm::vec2& position) const;

	[[nodiscard]] static float SmoothingKernel(const float radius, const float distance);
	[[nodiscard]] static float SmoothingKernelDerivative(const float radius, const float distance);
	[[nodiscard]] static float SharpSmoothingKernel(const float radius, const float distance);
	[[nodiscard]] static float SharpSmoothingKernelDerivative(const float radius, const float distance);
	[[nodiscard]] glm::vec2	   CalculatePressureForce(const glm::vec2& pos) const;
	float					   ConvertDensityToPressure(const float density) const;

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
	float								 m_ParticleMass;
	bool								 m_bRunSimulation;
	bool								 m_bGravitation;
	mutable bool						 m_bScheduledSetup;
	mutable ::Utils::HighResolutionTimer m_Timer;
};
} // namespace GLEngine::GLRenderer