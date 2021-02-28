#pragma once

namespace GLEngine::Renderer::PhysicalProperties {
// sources: [1] https://pixelandpoly.com/ior.html
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
