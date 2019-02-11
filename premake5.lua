workspace "Engine"
	architecture "x64"

	configurations{
		"Debug",
		"Release"
	}

include "premakeDefines.lua"

IncludeDir = {}

include "Core"
include "Sandbox"
--include "GLEngine"