#pragma once

#include <Renderer/RendererApi.h>

#include <Renderer/Colours.h>
#include <Renderer/Components/ForceLinkComponentTU.h>

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

DECLARE_FORCE_LINK_COMPONENT_TU(EnsureLightComponentsRegistered);

} // namespace GLEngine::Renderer
