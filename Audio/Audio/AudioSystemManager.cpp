#include <AudioStdAfx.h>

#include <Audio/AudioSystemManager.h>

#include <fmod.hpp>

namespace GLEngine::Audio {

//=================================================================================
C_AudioSystemManager& C_AudioSystemManager::Instance()
{
	static C_AudioSystemManager instance; // Guaranteed to be destroyed.
										  // Instantiated on first use.
	return instance;
}

//=================================================================================
C_AudioSystemManager::C_AudioSystemManager()
	: m_System(nullptr)
{
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_System);
	if (IsError(result))
		return;
}

//=================================================================================
C_AudioSystemManager::~C_AudioSystemManager()
{
	if (!m_System)
		return;
	FMOD_RESULT result;


	result = m_System->close();
	if (IsError(result))
		return;
	result = m_System->release();
	if (IsError(result))
		return;
}

//=================================================================================
bool C_AudioSystemManager::IsError(const FMOD_RESULT result) const
{
	if (result != FMOD_OK)
	{
		CORE_LOG(E_Level::Error, E_Context::Audio, "Unable to initialize audio system. '{}'", result);
	}
	return result != FMOD_OK;
}

} // namespace GLEngine::Audio
