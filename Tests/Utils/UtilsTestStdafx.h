#pragma once

#include <Utils/Logging/LoggingMacros.h>

#include <Utils/STLAfx.h>

#include <glm/glm.hpp>
#include <CommonTestUtils/GlmPrinters.h>

#include <gtest/gtest.h>

#include <Core/CoreMacros.h>

GL_PUSH_WARNINGS()
#pragma warning(disable : 4996)
#ifdef __GNUC__
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif
#include <rttr/registration.h>
#include <rttr/registration_friend.h>
GL_POP_WARNINGS()