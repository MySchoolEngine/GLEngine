#pragma once

#include <Audio/AudioApi.h>

// fwd declaration
namespace FMOD {
class System;
class Sound;
class Channel;
} // namespace FMOD

enum FMOD_RESULT;

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
	void								 ActivateListener(std::shared_ptr<C_ListenerComponent>);
	std::shared_ptr<C_ListenerComponent> GetActiveListener() const;

private:
	C_AudioSystemManager();
	~C_AudioSystemManager();

	[[nodiscard]] bool IsError(const FMOD_RESULT result) const;
	void			   ReportError(const FMOD_RESULT result) const;

	FMOD::System*						 m_System;
	bool		  m_Initialized;
	std::shared_ptr<C_ListenerComponent> m_ActiveListener;
	bool								 m_Initialized;
};

} // namespace GLEngine::Audio
