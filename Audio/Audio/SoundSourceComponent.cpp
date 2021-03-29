#include <AudioStdAfx.h>

#include <Audio/AudioSystemManager.h>
#include <Audio/SoundSourceComponent.h>

#include <GUI/FileDialogWindow.h>
#include <GUI/GUIManager.h>

#include <Physics/Primitives/AABB.h>

#include <Core/CoreRegistrations.h>

#include <Utils/Parsing/MatrixParse.h>

#include <pugixml.hpp>

#include <fmod.hpp>
#include <imgui.h>

namespace GLEngine::Audio {

//=================================================================================
C_SoundSourceComponent::C_SoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner, const std::filesystem::path& file)
	: Entity::I_Component(std::move(owner))
	, m_Channel(nullptr)
	, m_Filepath(file)
	, m_Looped(true, "Looped sound")
	, m_PlayButton("PlaySound", std::bind(&C_SoundSourceComponent::PlaySound, this))
	, m_StopButton("StopSound", std::bind(&C_SoundSourceComponent::StopSound, this))
{
	SetMusic(file);
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
	FMOD_MODE mode;
	m_Sound->getMode(&mode);
	const auto isLooped = (mode & FMOD_LOOP_NORMAL) != 0;
	if (isLooped != m_Looped) // looping changed
	{
		if (m_Channel && m_Looped == false)
		{
			StopSound();
		}
		const auto soundModeResult = m_Sound->setMode(m_Looped ? FMOD_LOOP_NORMAL : FMOD_LOOP_OFF);
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
void C_SoundSourceComponent::DebugDrawGUI(GUI::C_GUIManager* guiMGR /*= nullptr*/)
{
	ImGui::Text("%s", fmt::format("File: {}", m_Filepath).c_str());
	ImGui::SameLine();
	if (guiMGR && ImGui::Button("Choose file"))
	{
		const auto soundSelectWindowGUID = NextGUID();
		auto*	   levelSelectWindwo	 = new GUI::C_FileDialogWindow(
			 ".ogg", "Select file",
			 [&, soundSelectWindowGUID, guiMGR](const std::filesystem::path& sound) {
				 SetMusic(sound);
				 guiMGR->DestroyWindow(soundSelectWindowGUID);
			 },
			 soundSelectWindowGUID, "./Sound");
		guiMGR->AddCustomWindow(levelSelectWindwo);
		levelSelectWindwo->SetVisible();
	}
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
	if (stopMusic != FMOD_RESULT::FMOD_OK)
	{
		CORE_LOG(E_Level::Error, E_Context::Audio, "Unable to initialize audio system. '{}'", stopMusic);
	}
}

//=================================================================================
void C_SoundSourceComponent::SetMusic(const std::filesystem::path& path)
{
	m_Sound = C_AudioSystemManager::Instance().GetSoundFile(path, true);
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
		if (auto loopAttrib = node.attribute("looped"))
		{
			listener->SetLooped(loopAttrib.as_bool());
		}

		return listener;
	}
};

REGISTER_GLOBAL_COMPONENT_BUILDER("AudioSource", C_SoundSourceBuilder);

} // namespace GLEngine::Audio
