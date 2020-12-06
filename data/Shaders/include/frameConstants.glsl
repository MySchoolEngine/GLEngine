//================================================================================
uniform frameConst{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 viewProjectionMatrix;
	vec4 CameraPosition;
	vec3 SunPos;
	float AmbientStrength;
	float Time;
} frame;
vec4 sunColor = vec4(.95,0.7,0.51,1);