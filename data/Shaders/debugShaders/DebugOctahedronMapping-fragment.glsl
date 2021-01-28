#version 430
#extension GL_ARB_bindless_texture : require

// per mesh
uniform sampler2D colorMap;

in vec3 modelPosition;

out vec4 fragColor;

// Returns +-1
vec2 signNotZero(vec2 v) {
	return vec2((v.x >= 0.0) ? +1.0 : -1.0, (v.y >= 0.0) ? +1.0 : -1.0);
}
// Assume normalized input. Output is on [-1, 1] for each component.
vec2 float32x3_to_oct(in vec3 v) {
	// Project the sphere onto the octahedron, and then onto the xy plane
	vec2 p = v.xy * (1.0 / (abs(v.x) + abs(v.y) + abs(v.z)));
	// Reflect the folds of the lower hemisphere over the diagonals
	return (v.z <= 0.0) ? ((1.0 - abs(p.yx)) * signNotZero(p)) : p;
}

//=================================================================================
void main()
{
	const vec2 uv = float32x3_to_oct(normalize(modelPosition))/2.0 +vec2(0.5, 0.5);

	fragColor = texture(colorMap, uv).xyza;
}