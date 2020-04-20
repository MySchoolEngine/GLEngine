struct Ray
{
	vec3 origin;
	vec3 dir;
};

struct Plane
{
	vec4 normal;
	vec3 center;
};

struct Disc
{
	Plane plane;
	float radiusSq;
};

bool RayPlaneIntersect(Ray ray, Plane plane, out float t)
{
	const vec3 rayOrigin = ray.origin - plane.center;
	t = -(dot(plane.normal, vec4(rayOrigin, 1.f))) / dot(plane.normal.xyz, ray.dir);
	return t > 0.0;
}

bool RayDiscIntersect(Ray ray, Disc disc, out float t, out float distSq)
{
	if(RayPlaneIntersect(ray, disc.plane, t))
	{
		vec3 intersection = ray.origin + ray.dir*t;
		vec3 toCenter = intersection - disc.plane.center;
		distSq = dot(toCenter, toCenter);
		return distSq <= disc.radiusSq;
	}
	return false;
}