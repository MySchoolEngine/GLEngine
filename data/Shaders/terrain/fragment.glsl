#version 430

//per mesh
layout (binding = 0) uniform sampler2D tex;
uniform vec4 modelColor;
uniform bool hasTexture;
uniform bool selected;

in vec2 uv;


out vec4 fragColor;

vec4 getNormal(){
	const vec2 size = vec2(0.3,0.0);
	const ivec3 off = ivec3(-1,0,1);
	
	vec4 wave = texture(tex, uv);
	float s11 = wave.x;
    float s01 = textureOffset(tex, uv, off.xy).x;
    float s21 = textureOffset(tex, uv, off.zy).x;
    float s10 = textureOffset(tex, uv, off.yx).x;
    float s12 = textureOffset(tex, uv, off.yz).x;
    vec3 va = normalize(vec3(size.xy,s21-s01));
    vec3 vb = normalize(vec3(size.yx,s12-s10));
    return vec4( cross(va,vb), 1 );
    /*
    float s11 = wave.x;
    float s01 = textureOffset(tex, uv, off.xy).x;
    float s21 = textureOffset(tex, uv, off.zy).x;
    float s10 = textureOffset(tex, uv, off.yx).x;
    float s12 = textureOffset(tex, uv, off.yz).x;
    float avg = s11 + s01 + s21 + s10+s12;
    avg/=5.0f;
    vec3 va = dFdx(vec3(uv.x*5, avg, uv.y*5));
    vec3 vb = dFdy(vec3(uv.x*5, avg, uv.y*5));
    vec4 bump = vec4( normalize(cross(va,vb)), s11 );*/

	//return vec4(normalize(cross(va,vb)),1);
}

//=================================================================================
void main()
{
	vec4 sun = vec4(0,50,5,1);
	vec4 normal = getNormal();
	vec4 upVec = vec4(0,1,0,1);

	vec4 MaterialDiffuseColor = vec4(1,0,0,1);
	float cosTheta;


	if(hasTexture){
		MaterialDiffuseColor = texture(tex, uv);
	}
	else{
		MaterialDiffuseColor = modelColor;
	}

	if(selected){
		MaterialDiffuseColor.r = texture(tex, uv).z;
	}

	cosTheta = dot(normal,normalize(sun));
	float steepnes = 1.0f - normal.y;

	vec4 MaterialAmbientColor = 0.3f * MaterialDiffuseColor;
	MaterialDiffuseColor = MaterialAmbientColor + MaterialDiffuseColor * vec4(1.0f, 1.0f, 0.8f, 1.0f) * cosTheta;

	if(steepnes < 0.2f){
		fragColor = vec4(1, 0,0,1);
	}
	if((steepnes < 0.7f) && (steepnes >= 0.2f)){
		fragColor = vec4(0, 1,0,1);
	}
	if(steepnes >= 0.7f){
		fragColor = vec4(0, 0,1,1);
	}
	fragColor = MaterialDiffuseColor;
}