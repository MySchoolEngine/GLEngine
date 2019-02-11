//=================================================================================
layout(std430, binding = 4) buffer splitFrustums
{
    vec2 frustum[4];
};

//=================================================================================
vec4 frustColors[5] = {
	vec4(1.0, 0.0, 0.0, 1.0),
	vec4(0.0, 1.0, 0.0, 1.0),
	vec4(0.0, 0.0, 1.0, 1.0),
	vec4(0.0, 1.0, 1.0, 1.0),
	vec4(1.0, 1.0, 0.0, 1.0)
};

//=================================================================================
bool inFrustum(vec2 frustum, float pos){
	if(pos > frustum.x && pos < frustum.y){
		return true;
	}
	return false;
}

//=================================================================================
vec4 getColorForFrustum(float pos){
	for(int i = 0; i < 4; ++i){
		if(inFrustum(frustum[i], pos)){
			return frustColors[i];
		}
	}
	return frustColors[4];
}

//=================================================================================
int getNumberFrustum(float pos){
	for(int i = 0; i < 4; ++i){
		if(inFrustum(frustum[i], pos)){
			return i;
		}
	}
	return 4;
}

//=================================================================================
bool isBorder(float pos){
	for(int i = 0; i < 4; ++i){
		if(pos == frustum[i].x || pos == frustum[i].y){
			return true;
		}
	}
	return false;
}