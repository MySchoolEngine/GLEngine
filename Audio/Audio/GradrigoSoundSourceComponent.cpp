#include <AudioStdAfx.h>

#include <Audio/AudioSystemManager.h>
#include <Audio/GradrigoSoundSourceComponent.h>

#include <Core/CoreRegistrations.h>

#include <Utils/Parsing/MatrixParse.h>

#include <pugixml.hpp>

#include <fmod.hpp>

namespace Gradrigo {
#include <gradrigo-interface.h>
}

#include <imgui.h>

FMOD_RESULT F_CALLBACK pcmreadcallback(FMOD_SOUND* sound, void* data, unsigned int datalen)
{
	auto*		 stereo16bitbuffer = (signed short*)data;
	auto*		 snd			   = (FMOD::Sound*)sound;
	void*		 userData;
	snd->getUserData(&(userData));
	auto* component = static_cast<GLEngine::Audio::C_GradrigoSoundSourceComponent*>(userData);

	return component->GetBuffer(data, datalen);
}

FMOD_RESULT F_CALLBACK pcmsetposcallback(FMOD_SOUND* /*sound*/, int /*subsound*/, unsigned int /*position*/, FMOD_TIMEUNIT /*postype*/)
{
	/*
		This is useful if the user calls Channel::setPosition and you want to seek your data accordingly.
	*/
	return FMOD_OK;
}

namespace GLEngine::Audio {


//=================================================================================
C_GradrigoSoundSourceComponent::C_GradrigoSoundSourceComponent(std::shared_ptr<Entity::I_Entity> owner)
	: T_Base(std::move(owner))
	, m_Gradrigo(-1)
	, m_Sound(nullptr)
{
	m_Gradrigo = Gradrigo::NewInstance(44100);
}

//=================================================================================
void C_GradrigoSoundSourceComponent::SetSourceFile(const std::filesystem::path& filepath)
{
	m_Filepath = filepath;
	std::ifstream stream(filepath);

	if (stream.fail())
	{
		CORE_LOG(E_Level::Error, E_Context::Audio, "Gradrigo file does't exists. {}", filepath);
		return;
	}

	const auto content = std::string(std::istream_iterator<char>(stream >> std::noskipws), std::istream_iterator<char>());
	stream.close();
	Gradrigo::ParseString(content.c_str(), m_Gradrigo);
	CORE_LOG(E_Level::Error, E_Context::Audio, "{}", Gradrigo::ReportBoxesAsJson(m_Gradrigo));

	Gradrigo::StartVoice("kaboom", m_Gradrigo);
	// todo need to know all the names
	// todo need to know final length

	FMOD_CREATESOUNDEXINFO exinfo;
	memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));
	exinfo.cbsize			 = sizeof(FMOD_CREATESOUNDEXINFO);
	exinfo.numchannels		 = 1;
	exinfo.defaultfrequency	 = 44100; /* Default playback rate of sound. */
	exinfo.decodebuffersize	 = 16384; /* Chunk size of stream update in samples. This will be the amount of data passed to the user callback. */
	exinfo.length			 = exinfo.defaultfrequency * exinfo.numchannels * sizeof(float) * 5; /* Length of PCM data in bytes of whole song (for Sound::getLength) */
	exinfo.format			 = FMOD_SOUND_FORMAT_PCMFLOAT;										 /* Data format of sound. */
	exinfo.userdata			 = this;
	exinfo.pcmreadcallback	 = pcmreadcallback;
	exinfo.pcmsetposcallback = pcmsetposcallback;


	m_Sound = C_AudioSystemManager::Instance().GetProgrammerSound(FMOD_OPENUSER | FMOD_LOOP_NORMAL | FMOD_3D, &exinfo);
}

//=================================================================================
FMOD::Sound* C_GradrigoSoundSourceComponent::GetSound() const
{
	return m_Sound;
}

//=================================================================================
void C_GradrigoSoundSourceComponent::DebugDrawGUI(GUI::C_GUIManager* guiMGR /*= nullptr*/)
{
	ImGui::Text("%s", fmt::format("Loaded file: {}", m_Filepath).c_str());
	T_Base::DebugDrawGUI(guiMGR);
}

//=================================================================================
[[nodiscard]] std::string_view C_GradrigoSoundSourceComponent::GetDebugComponentName() const
{
	return "Gradrigo sound source";
}

//=================================================================================
FMOD_RESULT C_GradrigoSoundSourceComponent::GetBuffer(void* data, unsigned int datalen) const
{
	auto* buffer = (float*)data;
	Gradrigo::GetBuffer(datalen / sizeof(float), buffer, m_Gradrigo);
	return FMOD_OK;
}

//=================================================================================
//=================================================================================
class C_GradrgioSoundSourceBuilder : public Entity::I_ComponenetBuilder {
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

		auto gradrigoComponent = std::make_shared<Audio::C_GradrigoSoundSourceComponent>(owner);
		gradrigoComponent->SetSourceFile(file);
		gradrigoComponent->SetComponentMatrix(Utils::Parsing::C_MatrixParser::ParseTransformation(node));
		if (auto loopAttrib = node.attribute("looped"))
		{
			gradrigoComponent->SetLooped(loopAttrib.as_bool());
		}

		return gradrigoComponent;
	}
};

REGISTER_GLOBAL_COMPONENT_BUILDER("GradrigoAudioSource", C_GradrgioSoundSourceBuilder);

} // namespace GLEngine::Audio
