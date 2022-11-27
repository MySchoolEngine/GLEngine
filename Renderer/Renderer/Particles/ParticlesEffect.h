#pragma once

#include <Renderer/RayCasting/Generator/Sampler.h>
#include <Renderer/RendererApi.h>

#include <Entity/IComponent.h>
#include <Entity/IEntity.h>

#include <rttr/registration.h>
#include <rttr/registration_friend.h>

namespace GLEngine::Renderer {

class I_DebugDraw;

// position
// velocity
// lifetime
struct Particle {
	glm::vec3 position;
	float	  lifetime; // s
	glm::vec3 velocity; // m/s
};

class RENDERER_API_EXPORT C_ParticleEffect : public Entity::I_Component {
public:
	C_ParticleEffect(std::shared_ptr<Entity::I_Entity> owner);
	C_ParticleEffect();
	~C_ParticleEffect(); // = default;
	virtual void Update(float dt) override;
	virtual void DebugDrawGUI() override;

	void DebugDraw(Renderer::I_DebugDraw* dd) const;

	//=================================================================================
	Entity::E_ComponentType		GetType() const override;
	bool						HasDebugDrawGUI() const override;
	std::string_view			GetDebugComponentName() const override;
	Physics::Primitives::S_AABB GetAABB() const override;
	void						OnEvent(Core::I_Event& event) override;


	RTTR_ENABLE(I_Component);
	RTTR_REGISTRATION_FRIEND;

private:
	void				  SpawnParticle(Particle& particle);
	std::vector<Particle> m_Particles;
	C_STDSampler		  m_Rnd;

	float m_MinSpead = .1f;
	float m_MaxSpeed = 1.f;
};
} // namespace GLEngine::Renderer

namespace GLEngine {

template <> class ComponenetBase<Entity::E_ComponentType::Particle> {
public:
	using type = Renderer::C_ParticleEffect;
};
} // namespace GLEngine
