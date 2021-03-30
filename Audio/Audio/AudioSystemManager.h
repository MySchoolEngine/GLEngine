#pragma once

#include <Audio/AudioApi.h>

// fwd declaration
namespace FMOD {
class System;
class Sound;
class Channel;
} // namespace FMOD

typedef unsigned int FMOD_MODE;
struct FMOD_CREATESOUNDEXINFO;

namespace GLEngine::Audio {

class C_ListenerComponent;

class AUDIO_API_EXPORT C_AudioSystemManager {
public:
	// Singleton stuff
	C_AudioSystemManager(C_AudioSystemManager const&)  = delete;
	void									   operator=(C_AudioSystemManager const&) = delete;
	[[nodiscard]] static C_AudioSystemManager& Instance();

	void Update();
	void Done();

	[[nodiscard]] FMOD::Sound* GetSoundFile(const std::filesystem::path& path, bool sound3D);
	[[nodiscard]] FMOD::Sound* GetProgrammerSound(FMOD_MODE mode, FMOD_CREATESOUNDEXINFO* exinfo);

	void								 ActivateListener(std::shared_ptr<C_ListenerComponent>);
	std::shared_ptr<C_ListenerComponent> GetActiveListener() const;

	[[nodiscard]] FMOD::Channel* PlaySound(FMOD::Sound* sound);

private:
	C_AudioSystemManager();
	~C_AudioSystemManager();

	[[nodiscard]] bool IsError(const FMOD_RESULT result) const;
	void			   ReportError(const FMOD_RESULT result) const;

	FMOD::System*						 m_System;
	std::shared_ptr<C_ListenerComponent> m_ActiveListener;
	bool								 m_Initialized;

	std::map<std::filesystem::path, FMOD::Sound*> m_SoundDatabase;
	std::vector<FMOD::Sound*>					  m_ProgrammerSounds;
};

} // namespace GLEngine::Audio
