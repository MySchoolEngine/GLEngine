#include <AudioStdAfx.h>

#include <Audio/AudioSystemManager.h>
#include <Audio/SoundSourceComponent.h>

#include <Physics/Primitives/AABB.h>

#include <fmod.hpp>


namespace GLEngine::Audio {

//=================================================================================
C_SoundSourceComponent::C_SoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner)
	: Entity::I_Component(std::move(owner))
	, m_Channel(nullptr)
	, m_Looped(true, "Looped sound")
	, m_PlayButton("PlaySound", std::bind(&C_SoundSourceComponent::PlaySound, this))
	, m_StopButton("StopSound", std::bind(&C_SoundSourceComponent::StopSound, this))
	, m_LastPosition(GetPosition())
{
}

//=================================================================================
void C_SoundSourceComponent::Update()
{
	auto* sound = GetSound();
	if (!sound)
	{
		return;
	}

	FMOD_MODE mode;
	sound->getMode(&mode);
	const auto isLooped = (mode & FMOD_LOOP_NORMAL) != 0;
	if (isLooped != m_Looped) // looping changed
	{
		if (m_Channel && m_Looped == false)
		{
			StopSound();
		}
		const auto soundModeResult = sound->setMode(m_Looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
		if (soundModeResult != FMOD_RESULT::FMOD_OK)
		{
			CORE_LOG(E_Level::Error, E_Context::Audio, "Unable to initialize audio system. '{}'", soundModeResult);
		}
	}
	UpdateSoundSourcePosition();
}

//=================================================================================
glm::vec3 C_SoundSourceComponent::GetPosition() const
{
	return glm::vec3(GetComponentModelMatrix()[3]);
}

//=================================================================================
[[nodiscard]] Entity::E_ComponentType C_SoundSourceComponent::GetType() const
{
	return Entity::E_ComponentType::AudioSource;
}

//=================================================================================
[[nodiscard]] bool C_SoundSourceComponent::HasDebugDrawGUI() const
{
	return true;
}

//=================================================================================
[[nodiscard]] std::string_view C_SoundSourceComponent::GetDebugComponentName() const
{
	return "Audio source";
}

//=================================================================================
[[nodiscard]] Physics::Primitives::S_AABB C_SoundSourceComponent::GetAABB() const
{
	Physics::Primitives::S_AABB ret;
	ret.Add(GetPosition());
	return ret;
}

//=================================================================================
void C_SoundSourceComponent::DebugDrawGUI(GUI::C_GUIManager* guiMGR /*= nullptr*/)
{
	m_Looped.Draw();
	if (IsPlaying())
	{
		m_StopButton.Draw();
	}
	else
	{
		m_PlayButton.Draw();
	}
}

//=================================================================================
void C_SoundSourceComponent::PlaySound()
{
	m_Channel = C_AudioSystemManager::Instance().PlaySound(GetSound());
	UpdateSoundSourcePosition();
}

//=================================================================================
void C_SoundSourceComponent::UpdateSoundSourcePosition()
{
	if (m_Channel)
	{
		const auto position			 = GetPosition();
		const auto velocity			 = (position - m_LastPosition);
		const auto pos				 = ToFMOD(position);
		const auto vel				 = ToFMOD(velocity);
		const auto channelAttributes = m_Channel->set3DAttributes(&pos, &vel);
		if (IsError(channelAttributes))
			return;

		m_LastPosition = position;
	}
}

//=================================================================================
void C_SoundSourceComponent::SetLooped(bool looped)
{
	m_Looped = looped;
}

//=================================================================================
bool C_SoundSourceComponent::IsPlaying() const
{
	bool isPlaying = false;
	m_Channel->isPlaying(&isPlaying);
	return m_Channel && isPlaying;
}

//=================================================================================
void C_SoundSourceComponent::StopSound()
{
	const auto stopMusic = m_Channel->stop();
	if (IsError(stopMusic))
		return;
}

} // namespace GLEngine::Audio
