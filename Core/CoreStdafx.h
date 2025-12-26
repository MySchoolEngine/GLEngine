#pragma once

#include <Utils/Logging/LoggingMacros.h>

#include <Utils/STLAfx.h>

#include <Core/GUID.h>

#include <Core/CoreMacros.h>
#include <Core/CoreApi.h>
#include <Core/CoreEnums.h>

GL_PUSH_WARNINGS()
#pragma warning(disable : 4996)
#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
GL_POP_WARNINGS()