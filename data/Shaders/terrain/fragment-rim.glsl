#version 430

//per mesh
uniform vec4 modelColor;

in vec4 normal;

out vec4 fragColor;

//=================================================================================
void main()
{
	vec4 sun = vec4(0,50,5,1);
	vec4 upVec = vec4(0,1,0,1);

	vec4 MaterialDiffuseColor = vec4(1,0,0,1);
	float cosTheta;


	MaterialDiffuseColor = modelColor;

	cosTheta = dot(normal,normalize(sun));

	vec4 MaterialAmbientColor = 0.6f * MaterialDiffuseColor;
	MaterialDiffuseColor = MaterialAmbientColor + MaterialDiffuseColor * vec4(1.0f, 1.0f, 0.8f, 1.0f) * cosTheta;
	fragColor = MaterialDiffuseColor;
}