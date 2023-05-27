#shader vertex
#version 450 core


layout (location = 0) in vec2 inPos;
layout (location = 1) in vec2 inTexCoords;

out vec2 texCoords;

void main()
{
    gl_Position = vec4(inPos.x, inPos.y, 0.0, 1.0); 
    texCoords = inTexCoords;
}  


#shader fragment
#version 450 core

out vec4 FragColor;
in vec2 texCoords;

uniform sampler2D screenTexture;

const float offset_x = 1.0f / 300.0f;
const float offset_y = 1.0f / 300.0f; 

vec2 offsets[9] = vec2[]
(
    vec2(-offset_x,  offset_y), vec2( 0.0f,    offset_y), vec2( offset_x,  offset_y),
    vec2(-offset_x,  0.0f),     vec2( 0.0f,    0.0f),     vec2( offset_x,  0.0f),
    vec2(-offset_x, -offset_y), vec2( 0.0f,   -offset_y), vec2( offset_x, -offset_y) 
);

float kernel[9] = float[](
    1.0 / 16, 2.0 / 16, 1.0 / 16,
    2.0 / 16, 4.0 / 16, 2.0 / 16,
    1.0 / 16, 2.0 / 16, 1.0 / 16  
);

void main()
{
    vec4 color = vec4(0.0f);
    vec3 samplearray[9];

    for(int i = 0; i < 9; i++)
        samplearray[i] = vec3(texture(screenTexture, texCoords.st + offsets[i]));

    for(int i = 0; i < 9; i++)
            color += vec4(samplearray[i] * kernel[i], 0.0f);

    FragColor = vec4(vec3(color), 1.0f);
}