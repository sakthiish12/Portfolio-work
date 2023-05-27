#version 450

layout(location = 0) in float near;
layout(location = 1) in float far;
layout(location = 2) in vec3 nearPoint;
layout(location = 3) in vec3 farPoint;
layout(location = 4) in mat4 fragView;
layout(location = 8) in mat4 fragProj;
layout(location = 0) out vec4 outColor;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    const vec2 coord = fragPos3D.xz * scale;
    const vec2 derivative = fwidth(coord);
    const vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    const float line = min(grid.x, grid.y);
    const float minimumz = min(derivative.y, 1);
    const float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.1, 0.1, 0.1, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
        color.y = 1.0;
    // x axis
    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
        color.x = 1.0;
    return color;
}
float computeDepth(vec3 pos) {
    const vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    return (clip_space_pos.z / clip_space_pos.w);
}
float computeLinearDepth(vec3 pos) {
    const vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    const float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
    const float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
    return linearDepth / far; // normalize
}

void main() {
    const float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    const vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    const float linearDepth = computeLinearDepth(fragPos3D);
    const float fading = max(0, (0.5 - linearDepth));

    outColor = (grid(fragPos3D, 2, true) + grid(fragPos3D, 2, true))* float(t > 0) * fading; // adding multiple resolution for the grid

    if (outColor.a == 0.0) discard;
}