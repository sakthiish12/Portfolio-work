#version 450
layout (location = 0) out vec4 outColor;

layout (push_constant) uniform ParticlePushConstant {
	mat4 modelMatrix;
	mat4 normalMatrix;
    vec4 pos;
    vec4 color;
    float size;
} push;

layout (binding = 0) uniform sampler2D texParticle;

void main() {
    outColor = push.color;
	outColor.a = 1; 
}
