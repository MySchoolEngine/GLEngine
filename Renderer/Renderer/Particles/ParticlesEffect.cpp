#include <RendererStdAfx.h>

#include <Renderer/Colours.h>
#include <Renderer/DebugDraw.h>
#include <Renderer/Particles/ParticlesEffect.h>
#include <Renderer/RayCasting/Sampling.h>

#include <GUI/ReflectionGUI.h>

#include <Physics/Primitives/AABB.h>

#include <Utils/Reflection/Metadata.h>

#include <rttr/registration>

#pragma region registration
RTTR_REGISTRATION
{
	using namespace GLEngine::Renderer;
	using namespace Utils::Reflection;

	rttr::registration::class_<C_ParticleEffect>("C_ParticleEffect")
		.constructor<std::shared_ptr<GLEngine::Entity::I_Entity>>()(rttr::policy::ctor::as_std_shared_ptr)
		.constructor<>()(rttr::policy::ctor::as_std_shared_ptr)
		.property("MinSpeed", &C_ParticleEffect::m_MinSpead)(rttr::policy::prop::bind_as_ptr, RegisterMetaclass<MetaGUI::Slider>(),
															 RegisterMetamember<UI::Slider::Name>("Min speed"), RegisterMetamember<UI::Slider::Min>(0.1f),
															 RegisterMetamember<UI::Slider::Max>(10.0f), RegisterMetamember<SerializationCls::DerefSerialize>(true))
		.property("MaxSpeed", &C_ParticleEffect::m_MaxSpeed)(rttr::policy::prop::bind_as_ptr, RegisterMetaclass<MetaGUI::Slider>(),
															 RegisterMetamember<UI::Slider::Name>("Max speed"), RegisterMetamember<UI::Slider::Min>(0.1f),
															 RegisterMetamember<UI::Slider::Max>(10.0f), RegisterMetamember<SerializationCls::DerefSerialize>(true));

	rttr::type::register_wrapper_converter_for_base_classes<std::shared_ptr<C_ParticleEffect>>();
	rttr::type::register_converter_func([](std::shared_ptr<C_ParticleEffect> ptr, bool& ok) -> std::shared_ptr<GLEngine::Entity::I_Component> {
		ok = true;
		return std::static_pointer_cast<GLEngine::Entity::I_Component>(ptr);
	});
}
#pragma endregion

namespace GLEngine::Renderer {

//=================================================================================
C_ParticleEffect::C_ParticleEffect(std::shared_ptr<Entity::I_Entity> owner)
	: I_Component(owner)
	, m_Rnd(0.f, 1.f)
{
	m_Particles.resize(100);
	for (auto& particle : m_Particles)
	{
		SpawnParticle(particle);
	}
}

//=================================================================================
C_ParticleEffect::C_ParticleEffect()
	: I_Component(nullptr)
	, m_Rnd(0.f, 1.f)
{
	m_Particles.resize(100);
	for (auto& particle : m_Particles)
	{
		SpawnParticle(particle);
	}
}

//=================================================================================
C_ParticleEffect::~C_ParticleEffect() = default;

//=================================================================================
void C_ParticleEffect::Update(float dt)
{
	for (auto& particle : m_Particles)
	{
		particle.lifetime -= dt;
		particle.position += dt * particle.velocity;
		if (particle.lifetime < 0.f)
		{
			SpawnParticle(particle);
		}
	}
}

//=================================================================================
void C_ParticleEffect::SpawnParticle(Particle& particle)
{
	particle.position		  = m_Rnd.GetV3();
	const glm::vec3 direction = UniformSampleHemisphere(m_Rnd.GetV2());
	particle.velocity		  = glm::vec3{direction.x, direction.z, direction.y} * (m_MinSpead + (m_MaxSpeed - m_MinSpead) * m_Rnd.GetD());
	particle.lifetime		  = 10 * m_Rnd.GetD();
}

//=================================================================================
Entity::E_ComponentType C_ParticleEffect::GetType() const
{
	return Entity::E_ComponentType::Particle;
}

//=================================================================================
bool C_ParticleEffect::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
std::string_view C_ParticleEffect::GetDebugComponentName() const
{
	return "ParticlesEffect";
}

//=================================================================================
Physics::Primitives::S_AABB C_ParticleEffect::GetAABB() const
{
	return {};
}

//=================================================================================
void C_ParticleEffect::OnEvent(Core::I_Event& event)
{
}

//=================================================================================
void C_ParticleEffect::DebugDraw(Renderer::I_DebugDraw* dd) const
{
	for (auto& particle : m_Particles)
	{
		if (particle.lifetime >= 0.f)
			dd->DrawPoint(particle.position, Colours::yellow);
	}
}

//=================================================================================
void C_ParticleEffect::DebugDrawGUI()
{
	rttr::instance obj(*this);
	GUI::DrawAllPropertyGUI(obj);
}

} // namespace GLEngine::Renderer
