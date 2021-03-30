#pragma once

enum FMOD_RESULT;
namespace GLEngine::Audio {
//=================================================================================
bool IsError(const FMOD_RESULT result);

//=================================================================================
void ReportError(const FMOD_RESULT result);
}

