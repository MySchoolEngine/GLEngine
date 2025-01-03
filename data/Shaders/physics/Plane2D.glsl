struct Plane2D
{
	vec2 Normal;
	vec2 Position;
};

vec2 GetDirection(const Plane2D plane)
{
	return vec2(plane.Normal.y, -plane.Normal.x);
}

float DistanceToPlane(const Plane2D plane, vec2 point)
{
	const vec2 dir = GetDirection(plane);
	const vec2 v = point-plane.Position;
	const float d = dot(v, dir);
	return length(plane.Position + dir * d - point);
}