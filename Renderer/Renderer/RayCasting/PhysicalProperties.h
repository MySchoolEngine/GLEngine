#pragma once

#include <Renderer/Colours.h>

namespace GLEngine::Renderer::PhysicalProperties {
// sources: [1] https://pixelandpoly.com/ior.html
//          [2] https://physicallybased.info/
namespace IOR {
static const auto Air	= 1.f;	  //[1]
static const auto Water = 1.325f; //< ~35°C [1]
static const auto Ice	= 1.309f; //[1]
static const auto Glass = 1.5f;	  //[1]
static const auto Beer	= 1.345f; //[1]
// metals
static const auto Gold	   = 0.470f; //[1]
static const auto Iron	   = 2.950f; //[1]
static const auto Lead	   = 2.01f;	 //[1]
static const auto Titanium = 2.16f;	 //[1]
static const auto Nickel   = 1.08f;	 //[1]
} // namespace IOR
} // namespace GLEngine::Renderer::PhysicalProperties

namespace GLEngine::Colours {
static const auto Water = Colours::T_Colour(1.000f, 1.000f, 1.000f); //[2]
static const auto Ice	= Colours::T_Colour(1.000f, 1.000f, 1.000f); //[2]
static const auto Glass = Colours::T_Colour(1.000f, 1.000f, 1.000f); //[2]
// metals
static const auto Gold	   = Colours::T_Colour(0.944f, 0.776f, 0.373f); //[2]
static const auto Iron	   = Colours::T_Colour(0.531f, 0.512f, 0.496f); //[2]
static const auto Lead	   = Colours::T_Colour(0.632f, 0.626f, 0.641f); //[2]
static const auto Titanium = Colours::T_Colour(0.616f, 0.582f, 0.544f); //[2]
static const auto Nickel   = Colours::T_Colour(0.649f, 0.610f, 0.541f); //[2]
} // namespace GLEngine::Colours
