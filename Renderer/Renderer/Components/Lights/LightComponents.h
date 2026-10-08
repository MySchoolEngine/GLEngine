#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Colours.h>

namespace GLEngine::Renderer {

struct S_PointLight {
	float			  intensity = 1.f;
	Colours::T_Colour color		= Colours::white;
};

struct S_AreaLight {
	float			  width			= 1.f;
	float			  height		= 1.f;
	Colours::T_Colour diffuseColor	= Colours::white;
	Colours::T_Colour specularColor = Colours::white;
};

struct S_SunLight {
	float			  sunX				= 0.437f;
	float			  sunY				= 0.056f;
	float			  sunZ				= -0.347f;
	Colours::T_Colour sunColor			= Colours::white;
	float			  asymmetricFactor	= 0.95f;
	float			  sunDiscMultiplier = 1.f;
};

// These light structs are entirely header-defined, so nothing outside this translation unit
// ever references an out-of-line symbol from LightComponents.cpp - in a static-library link
// (e.g. RendererTest on MSVC links RendererStatic, not the Renderer DLL), the linker would then
// drop that whole object file as unused, and its RTTR_REGISTRATION/C_ComponentRegistry static
// initializers would never run. Calling this once forces the TU to be linked in, which is
// sufficient for the rest of its static initializers to run too. Not needed in the real
// Renderer.dll build, where every linked TU's static init runs unconditionally at load time.
RENDERER_API_EXPORT void EnsureLightComponentsRegistered();

} // namespace GLEngine::Renderer
