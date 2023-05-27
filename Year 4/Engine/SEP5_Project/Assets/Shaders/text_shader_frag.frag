#version 450

layout (location = 1) in vec2 inUV;

layout (binding = 0) uniform sampler2D testFont;

layout (location = 0) out vec4 outFragColor;

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
    const vec4 texel = texture(testFont,inUV);
    outFragColor.rgb = texel.rgb * push.colorAndOpacity.rgb * push.brightnessFactor * push.colorIntensityFactors.rgb;
    outFragColor.a = texel.a * push.colorAndOpacity.a * push.brightnessFactor * push.colorIntensityFactors.a;
}