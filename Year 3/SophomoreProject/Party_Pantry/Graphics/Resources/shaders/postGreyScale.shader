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

void main()
{
    FragColor = texture(screenTexture, texCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
}