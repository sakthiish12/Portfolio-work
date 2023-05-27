#version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 color;

layout(location = 0) out vec3 outColor;

layout(binding = 0) uniform sampler2D texParticle;

layout(push_constant) uniform ParticlePushConstant {
	mat4 modelMatrix;
	mat4 normalMatrix;
    vec4 pos;
    vec4 color;
    float size;
} push;
/*
mat4 rotateMatrix(float angle) {
    mat4 rotationMatrix = mat4(1.0);
    rotationMatrix[0][0] = cos(angle);
    rotationMatrix[0][1] = sin(angle);
    rotationMatrix[1][0] = -sin(angle);
    rotationMatrix[1][1] = cos(angle);
    return rotationMatrix;
}*/

void main() {
    gl_Position = push.modelMatrix * vec4( position.xyz , 1.0f ); // W2C
	
}

