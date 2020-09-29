// for 680, 550, 440
const vec3 SeaLevelScatterCoef = vec3(5.8, 13.5, 33.1);
const float Hr = 1/8000; // Hr = 8 km
const float Hm = 1/1200; // Hm = 1.2 km
const float scatter2ExtinctionRatio = 0.9; // B^s/B^e = 0.9 according to paper, only areosols absorbs incident light

const float earthRadius = 6378*1000; // 6378 setri se osle
const float atmosphereThickness = 60 * 1000; // 60km atmosphere

vec3 GetPlanetCenter()
{
	// earth center is exactly under cameraposition
	vec3 camPosition = frame.CameraPosition.xyz/frame.CameraPosition.w;
	camPosition.y = -earthRadius;
	return camPosition;
}

float GetAltitude(vec3 point)
{
	return distance(point, GetPlanetCenter()) - earthRadius;
}

vec3 GetAirScatteringCoef(float altitude)
{
	return SeaLevelScatterCoef * exp(- altitude * Hr);
}

vec3 GetAirExtinctionCoef(float altitude)
{
	return GetAirScatteringCoef(altitude);
}

vec3 GetAreosolAbsorbtionCoef(float altitude)
{
	return SeaLevelScatterCoef * exp(- altitude * Hm);
}

vec3 GetAreosolExtinctionCoef(float altitude)
{
	return GetAreosolAbsorbtionCoef(altitude) / scatter2ExtinctionRatio;
}

vec3 GetTotalExtinctionCoefAlongRay(Ray r, float rayLen)
{
	const int numSamples = 10;
	vec3 samplingPoint = r.origin;
	float stepSize = rayLen / (numSamples - 1);

	vec3 Be = vec3(0, 0, 0);
	for ( int i = 0; i < numSamples; ++i)
	{
		const float altitude = GetAltitude(samplingPoint); 
		const vec3 BeR = GetAirExtinctionCoef(altitude);
		const vec3 BeM = GetAreosolExtinctionCoef(altitude);
		Be += stepSize * (BeR + BeM);
		samplingPoint += r.dir * stepSize;
	}

	return Be;
}

vec3 Transmittance(Ray r, float rayLen)
{
	return exp(-GetTotalExtinctionCoefAlongRay(r, rayLen));
}