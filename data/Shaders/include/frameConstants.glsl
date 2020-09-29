//================================================================================
uniform frameConst{
	mat4 projectionMatrix;
	mat4 viewMatrix;
	mat4 viewProjectionMatrix;
	vec4 CameraPosition;
	vec3 SunPos;
	float AmbientStrength;
	float Time;
	float zNear;
	float zFar;
} frame;
vec4 sunColor = vec4(.95,0.7,0.51,1);

float toLinearDepth(float sampledDepth)
{
    float z_n = 2.0 * sampledDepth - 1.0;
    float z_e = 2.0 * frame.zNear * frame.zFar / (frame.zFar + frame.zNear - z_n * (frame.zFar - frame.zNear));

    return z_e;
}