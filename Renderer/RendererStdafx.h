#pragma once

#include <Utils/Logging/LoggingMacros.h>

#include <Utils/STLAfx.h>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/hash.hpp>

#include <Core/CoreMacros.h>

#include <Renderer/RendererApi.h>
#include <Renderer/Colours.h>

GL_PUSH_WARNINGS()
#pragma warning(disable : 4996)
#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
GL_POP_WARNINGS()