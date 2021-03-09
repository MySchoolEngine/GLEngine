#version 430

// per mesh
uniform vec4 modelColor;

out vec4 fragColor;

flat in int number;

//=================================================================================
void main()
{

	switch (number)
	{
	case 0:
		fragColor = vec4(0, 0, 0, 0);
		return;
	case 1:
		fragColor = vec4(0, 1, 1, 0);
		return;
	case 2:
		fragColor = vec4(0, 0, 1, 0);
		return;
	case 3:
		fragColor = vec4(0, 1, 0, 0);
		return;
	}

	fragColor = modelColor; // texture(tex, texCoordOUT);//vec4(texCoordOUT.xy, 0.0, 0.0);//MaterialDiffuseColor;
}