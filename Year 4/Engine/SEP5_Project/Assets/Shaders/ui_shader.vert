#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bittangent;
layout(location = 6) in ivec4 bone_idxs;
layout(location = 7) in vec4 bone_weights;

layout(location = 0) out vec2 uvOut;
layout(location = 1) out vec2 posOut;

layout(push_constant) uniform Push {
	vec4 posAndScale; //pos.x pos.y scale.x scale.y
	vec4 flickerColor;
	int textureToUse; //1~3
	float percentage;
	float flickerOpacity;
	float opacity;
	int isVertical;
	int uiType; 
	float brightness;
	float rotationRad;
	vec4 colorIntensityFactors;
	// 1 - button | 2 - font | 3 - image 
	//| 4 - progressbarBG | 5 - progressbarFG | 6 - progressbarBorder
	//| 7 - sliderBG | 8 - sliderKnob | 9 - toggleBG | 10 - toggleCheckmark
} push;


void main() 
{
	uvOut = vec2(uv.s,1.0-uv.t);
	const vec2 originalVertexPos = vec2(position.x,position.y); // move to top left as origin
	float flippedScale = 1.0;
	
	if(push.posAndScale.z < 0.0f || push.posAndScale.w < 0.0f)
	{
		flippedScale = -1.0;
	}
	
	const mat4 scale = mat4(
        vec4(push.posAndScale.z, 0.0, 0.0, 0.0),
        vec4(0.0, push.posAndScale.w, 0.0, 0.0),
        vec4(0.0, 0.0, flippedScale, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0));
		
	const mat4 rotation = mat4(vec4(cos(push.rotationRad),-sin(push.rotationRad),0.0,0.0),
		vec4(sin(push.rotationRad),cos(push.rotationRad),0.0,0.0),
		vec4(0.0,0.0,1.0,0.0),
		vec4(0.0,0.0,0.0,1.0));
		

	const mat4 translate = mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(push.posAndScale.x * 2.0 - 1.0,push. posAndScale.y * -2.0 +1.0,0.0, 1.0));
		
	gl_Position = translate*scale*rotation*vec4(originalVertexPos.xy,0.0,1.0);
	posOut = originalVertexPos;
}
