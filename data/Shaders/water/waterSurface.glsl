float waterHeight(vec2 pos, float time)
{
	return sin(pos.x * 20 + time) * cos(pos.y * 5 + time) / 2;
}

vec3 waterNormal(vec2 pos, float time)
{
	const float resolution = 1.0 / 512.0;
	const float meshSize   = 6.0;
	float		y[5];
	waterHeight(vec2(pos.x + resolution, pos.y), time);
	y[0] = waterHeight(vec2(pos.x + resolution, pos.y), time);
	y[1] = waterHeight(vec2(pos.x, pos.y + resolution), time);
	y[2] = waterHeight(vec2(pos.x - resolution, pos.y), time);
	y[3] = waterHeight(vec2(pos.x, pos.y - resolution), time);

	y[4] = waterHeight(pos, time);

	vec3 Normal = normalize(vec3(y[2] - y[0], meshSize / 512.0 * 2.0, y[1] - y[3]));

	return Normal;
}