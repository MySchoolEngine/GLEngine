#include <AudioStdAfx.h>

#include <Audio/FMODHelpers.h>

#include <fmod.hpp>

namespace GLEngine::Audio {

//=================================================================================
bool IsError(const FMOD_RESULT result)
{
	ReportError(result);
	return result != FMOD_OK;
}

//=================================================================================
void ReportError(const FMOD_RESULT result)
{
	if (result != FMOD_OK)
	{
		CORE_LOG(E_Level::Error, E_Context::Audio, "Failed FMOD operation. '{}'", result);
	}
}

}
