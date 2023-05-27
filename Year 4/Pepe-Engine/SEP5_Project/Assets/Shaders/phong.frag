#version 450

layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec3 inView;
layout (location = 2) in vec3 inLight;
layout (location = 3) in vec3 inColor;

layout (location = 0) out vec4 outColor;

void main() {
    const vec3 cr = inColor; // diffuse reflectance
    const vec3 cl = vec3(0.7); // light intensity
    const vec3 ca = vec3(0.2); // ambient light
    const vec3 cp = vec3(0.35); // highlight color
    const float p = 32.0; // phong exponent

    // For metallic objects highlights take on a metallic color

	const vec3 N = normalize(inNormal);
    const vec3 L = normalize(inLight);
    const vec3 V = normalize(inView);
    const vec3 R = reflect(-L, N);

    const vec3 diffuse = cr * min(ca + cl * max(dot(N, L), 0.0), vec3(1.0));
    const vec3 specular = cl * cp * pow(max(dot(R, V), 0.0), p);

	outColor = vec4(diffuse + specular, 1.0);
}