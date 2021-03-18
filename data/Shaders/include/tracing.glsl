struct Ray {
	vec3 origin;
	vec3 dir;
};

struct Plane {
	vec4 normal; // TODO: Should not be vec4 but vec3
	vec3 center;
};

struct Rect {
	Plane plane;
	vec3  DirX;
	float width;
	vec3  DirY;
	float height;
};

struct Disc {
	Plane plane;
	float radiusSq;
};

struct Sphere {
	vec3  center;
	float radius;
};

bool RayPlaneIntersect(Ray ray, Plane plane, out float t)
{
	const vec3 rayOrigin = ray.origin - plane.center;
	t					 = -(dot(plane.normal, vec4(rayOrigin, 1.f))) / dot(plane.normal.xyz, ray.dir);
	return t > 0.0;
}

bool RayDiscIntersect(Ray ray, Disc disc, out float t, out float distSq)
{
	if (RayPlaneIntersect(ray, disc.plane, t))
	{
		vec3 intersection = ray.origin + ray.dir * t;
		vec3 toCenter	  = intersection - disc.plane.center;
		distSq			  = dot(toCenter, toCenter);
		return distSq <= disc.radiusSq;
	}
	return false;
}

// t returns:
// x: distance to first hit (negative value means youre inside)
// y: distance to the second hit
// z: distnace through sphere
// x<=y && z<= distance(x, y)
bool Intersect(const Ray ray, const Sphere sphere, out vec3 t)
{
	const vec3	L	  = sphere.center - ray.origin;
	const float theta = dot(L, ray.dir);
	// if(theta <= 0) return false; I need to make it work from inside

	const float d2 = dot(L, L) - theta * theta;
	const float r2 = sphere.radius * sphere.radius;
	if (d2 > r2)
		return false;

	const float t1c = sqrt(r2 - d2);
	t.x				= theta - t1c;
	t.y				= theta + t1c;
	if (t.x < 0)
	{
		t.z = t.y;
	}
	else
	{
		t.z = t.y - t.x;
	}
	return true;
}
/*
 * Transforms screen space coordinates to clip space
 * Clip space have (0,0) in the middle of viewport
 */
Ray GetCameraRay(const vec2 clipSpaceCoord)
{
	const vec3 camPos = (frame.CameraPosition.xyz / frame.CameraPosition.w);

	vec4 rayEye = inverse(frame.projectionMatrix) * vec4(clipSpaceCoord, -1, 1);
	rayEye.z	= -1;
	rayEye.w	= 0;
	rayEye		= normalize(inverse(frame.viewMatrix) * rayEye);

	return Ray(camPos, normalize(rayEye.xyz));
}