// for 680, 550, 440
const vec3 SeaLevelScatterCoef = vec3(5.8, 13.5, 33.1) * pow(10.0, -6);
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

Sphere GetAtmosphereBoundary()
{
	return Sphere(GetPlanetCenter(), earthRadius + atmosphereThickness);
}

Sphere GetSunSphere()
{
	const vec3 planetCenter = GetPlanetCenter();
    return Sphere(planetCenter + normalize(frame.SunPos) * (earthRadius), 150000.0); 
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

vec3 AirMass(const Ray r, float rayLen)
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

vec3 Transmittance(const Ray r, float rayLen)
{
	return exp(-AirMass(r, rayLen));
}

float ReyleighPhaseFunction(float angle)
{
	const float k = 3.0/(16.0 * PI);
	return k * (1 + pow(angle, 2.0));
}

// Cornette-Shanks
float MiePhaseFunctionCS(float g, float angle)
{
	const float g2 = pow(g, 2.0);
	const float k = 3.0/(16.0 * PI);
	const float denom = (2 + g2) * pow((1 + g2 - 2 * g * angle), 3.0/ 2.0);
	return k * ((1- g2)*(1+ angle*angle)) / denom;
}


//http://www.csroc.org.tw/journal/JOC25-3/JOC25-3-2.pdf
// Xiao-Lei Fan
float MiePhaseFunctionXLF(float g, float angle)
{
	const float g2 = pow(g, 2.0);
	const float k = 3.0/(16.0 * PI);
	const float denom = (1 + g2 - 2 * g * angle);
	return k * ((1-g2)/(2+g2)) * ((1 + angle*angle) / denom) + g * angle;
}

vec3 InScatteredLight(vec3 y, vec3 v, vec3 s)
{
	const float altitude = GetAltitude(y); 
	float Mie = MiePhaseFunctionCS(0.2, dot(v, s));
	return GetAirExtinctionCoef(GetAltitude(y)) * ReyleighPhaseFunction(dot(v, s)) + GetAreosolExtinctionCoef(GetAltitude(y)) * MiePhaseFunctionCS(1, dot(v, s));
}

vec3 GetSkyRadiance(Ray r, float rayLen)
{
	const Sphere sun = GetSunSphere();
	const int numSamples = 1000;
	vec3 samplingPoint = r.origin;
	float stepSize = rayLen / (numSamples - 1);

	vec3 radiance = vec3(0, 0, 0);
	for ( int i = 0; i < numSamples; ++i)
	{
		radiance += stepSize * InScatteredLight(samplingPoint, r.dir, normalize(sun.center - samplingPoint)) * Transmittance(r, i * stepSize);
		samplingPoint += r.dir * stepSize;
	}

	return radiance;
}