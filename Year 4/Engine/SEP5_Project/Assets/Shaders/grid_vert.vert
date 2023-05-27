#version 450

layout(location = 0) out float near; //0.01
layout(location = 1) out float far; //100
layout(location = 2) out vec3 nearPoint;
layout(location = 3) out vec3 farPoint;

layout(location = 4) out mat4 fragView;
layout(location = 8) out mat4 fragProj;


layout(set = 0, binding = 0) uniform GlobalUbo{
	mat4 projection;
	mat4 view;
	mat4 invView;
} ubo;

// Grid position are in xy clipped space
const vec3 gridPlane[6] = vec3[] (
    vec3(1, 1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
    vec3(-1, -1, 0), vec3(1, 1, 0), vec3(1, -1, 0)
);

vec3 UnprojectPoint(float x, float y, float z, mat4 projection) {
    const mat4 projInv = inverse(projection);
    const vec4 unprojectedPoint =  ubo.invView * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main() {
	const vec3 p = gridPlane[gl_VertexIndex].xyz;
    nearPoint = UnprojectPoint(p.x, p.y, 0.0, ubo.projection).xyz; // unprojecting on the near plane
    farPoint = UnprojectPoint(p.x, p.y, 0.5, ubo.projection).xyz; // unprojecting on the far plane
		
	fragView = ubo.view;
	fragProj = ubo.projection;

	near = 0.01;
	far = 2.5;

    gl_Position = vec4(p, 1.0); // using directly the clipped coordinates
}
