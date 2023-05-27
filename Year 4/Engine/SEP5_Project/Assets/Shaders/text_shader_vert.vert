#version 450

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;

layout (location = 1) out vec2 outUV;

out gl_PerVertex 
{
	vec4 gl_Position;   
};

layout(push_constant) uniform Push {
    mat4 projection;
    mat4 view;
    vec4 colorIntensityFactors;
	vec4 colorAndOpacity;
    int textSpaceType;
    float brightnessFactor;
} push;

void main()
{
    if(push.textSpaceType == 0) //local
    {
        gl_Position = vec4(inPosition,0.0,1.0);
    }
    outUV = inUV;
}