#pragma once

enum FMOD_RESULT;
struct FMOD_VECTOR;
namespace GLEngine::Audio {
//=================================================================================
bool IsError(const FMOD_RESULT result);

//=================================================================================
void ReportError(const FMOD_RESULT result);

//=================================================================================
FMOD_VECTOR ToFMOD(const glm::vec3& vec);

}

