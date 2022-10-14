#include <RendererStdafx.h>

#include <Renderer/RayCasting/Light/BackgroundLight.h>

namespace GLEngine::Renderer::RayTracing {

//=================================================================================
C_BackgroundLight::C_BackgroundLight(const Colours::T_Colour& backgroundColor)
	: m_BackgroundColor(backgroundColor)
{
}

//=================================================================================
Colours::T_Colour C_BackgroundLight::SampleLi(const C_RayIntersection& intersection, I_Sampler* rnd, S_VisibilityTester& vis, float* pdf) const
{
	// float pdfSA;
	// auto  wil  = SampleUniformSphereW(Vec2f(rnd1, rnd2), &pdfSA);
	// oWig	   = aFrame.ToWorld(wil);
	// oLightDist = std::numeric_limits<float>::max();
	//
	// const float oCosThetaIn = wil.z;
	//
	// pdf = std::numeric_limits<float>::infinity();
	//
	// return mBackgroundColor * oCosThetaIn / pdfSA;
	return Le();
}

//=================================================================================
Colours::T_Colour C_BackgroundLight::Le() const
{
	return m_BackgroundColor;
}

}
