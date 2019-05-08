#version 430

#define dropletsNUM 100

layout (local_size_x = dropletsNUM, local_size_y = 1, local_size_z = 1) in;
layout (binding = 0, rgba32f) uniform image2D terrainPatch;

uniform int numDrops;

//================================================================================
layout (binding = 1) uniform rainData{
	ivec2[dropletsNUM] droplets;
} rain;

//=================================================================================
struct Droplet
{
  vec2 position;
  vec2 direction;
  float velocity;
  float water;
  float sediment;
};

vec3 GetGradientAndHeight(vec2 posf){
	const vec2 size = vec2(0.3,0.0);
	const ivec3 off = ivec3(-1,0,1);
	const ivec2 pos = ivec2(posf.x, posf.y);
	
	vec4 wave = imageLoad(terrainPatch, pos);
	float s11 = wave.x;
    float s01 = imageLoad(terrainPatch, pos+off.xy).x;//[-1,0]
    float s21 = imageLoad(terrainPatch, pos+ off.zy).x;//[1,0]
    float s10 = imageLoad(terrainPatch, pos+ off.yx).x;//[0,-1]
    float s12 = imageLoad(terrainPatch, pos+ off.yz).x;//[0,1]
	float gradX = s21-s01;
	float gradY = s12-s10;


    return vec3(gradX, gradY, wave.x);
}

//=================================================================================
void main()
{
	uint posX = gl_GlobalInvocationID.x;
	// simplest way to limit number of droplets I found
	if(posX >= numDrops){
		return;
	}

    Droplet drop;
    ivec2 posI = rain.droplets[posX];
    drop.position = vec2(posI.x, posI.y);
    drop.direction = vec2(0.0, 0.0);
    drop.velocity = 1.0f;// starting speed
    drop.water = 1.0f; // starting water
    drop.sediment = 0.0f;// cause it doesnt took any sediment yet

    for(int i = 0;i<10;++i){
    	drop.direction = normalize(GetGradientAndHeight(drop.position).xy);
		float val = imageLoad(terrainPatch, ivec2(drop.position)).x - 4.0f/255.0f;
		imageStore(terrainPatch, ivec2(drop.position), vec4(val, val, val, 1));

		drop.position += drop.direction;
	}

}  