#pragma once

#include <Renderer/Colours.h>
#include <Renderer/RendererApi.h>

#include <Core/CoreMacros.h>
#include <Core/Profiling/Profiler.h>

#include <Utils/Logging/LoggingMacros.h>
#include <Utils/STLAfx.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/hash.hpp>

GL_PUSH_WARNINGS()
#pragma warning(disable : 4996)
#pragma warning(disable : 4506)
#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
GL_POP_WARNINGS()