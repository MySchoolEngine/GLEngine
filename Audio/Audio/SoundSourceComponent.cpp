#include <AudioStdAfx.h>

#include <Audio/AudioSystemManager.h>
#include <Audio/SoundSourceComponent.h>

#include <Physics/Primitives/AABB.h>

#include <Core/CoreRegistrations.h>

#include <Utils/Parsing/MatrixParse.h>

#include <pugixml.hpp>

#include <fmod.hpp>

namespace GLEngine::Audio {

//=================================================================================
C_SoundSourceComponent::C_SoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner, const std::filesystem::path& file)
	: Entity::I_Component(std::move(owner))
	, m_Channel(nullptr)
	, m_Filepath(file)
	, m_Looped(true)
{
	m_Sound					   = C_AudioSystemManager::Instance().GetSoundFile(file, true);
	const auto soundModeResult = m_Sound->setMode(m_Looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
	if (soundModeResult != FMOD_RESULT::FMOD_OK)
	{
		CORE_LOG(E_Level::Error, E_Context::Audio, "Unable to initialize audio system. '{}'", soundModeResult);
		return;
	}
	PlaySound();
}

//=================================================================================
void C_SoundSourceComponent::Update()
{
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
	return Entity::E_ComponentType::AudioListener;
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
void C_SoundSourceComponent::DebugDrawGUI()
{
}

//=================================================================================
FMOD::Sound* C_SoundSourceComponent::GetSound() const
{
	return m_Sound;
}

//=================================================================================
void C_SoundSourceComponent::PlaySound()
{
	m_Channel = C_AudioSystemManager::Instance().PlaySound(m_Sound);
	UpdateSoundSourcePosition();
}

//=================================================================================
void C_SoundSourceComponent::UpdateSoundSourcePosition()
{
	if (m_Channel)
	{
		const auto		  position			= GetPosition();
		const FMOD_VECTOR pos				= {position.x, position.y, position.z};
		const FMOD_VECTOR vel				= {0.0f, 0.0f, 0.0f};
		const auto		  channelAttributes = m_Channel->set3DAttributes(&pos, &vel);
		if (channelAttributes != FMOD_RESULT::FMOD_OK)
		{
			CORE_LOG(E_Level::Error, E_Context::Audio, "Unable to initialize audio system. '{}'", channelAttributes);
			return;
		}
	}
}

//=================================================================================
//=================================================================================
class C_SoundSourceBuilder : public Entity::I_ComponenetBuilder {
public:
	virtual std::shared_ptr<Entity::I_Component> Build(const pugi::xml_node& node, std::shared_ptr<Entity::I_Entity> owner) override
	{
		std::filesystem::path file = "";
		if (auto fileAttrib = node.attribute("file"))
		{
			file = fileAttrib.value();
		}
		else
		{
			CORE_LOG(E_Level::Warning, E_Context::Audio, "Audio source without a sound");
		}
		auto listener = std::make_shared<Audio::C_SoundSourceComponent>(owner, file);
		listener->SetComponentMatrix(Utils::Parsing::C_MatrixParser::ParseTransformation(node));

		return listener;
	}
};

REGISTER_GLOBAL_COMPONENT_BUILDER("AudioSource", C_SoundSourceBuilder);

} // namespace GLEngine::Audio
