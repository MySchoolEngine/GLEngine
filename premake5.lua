workspace "Engine"
	architecture "x64"
	startproject "Sandbox"
	
	configurations{
		"Debug",
		"Release"
	}

include "premakeDefines.lua"

IncludeDir = {}
IncludeDir["GLFW"] = "vendor/GLFW/include"

include "vendor/GLFW"

include "Core"
include "Sandbox"
--include "GLEngine"