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

uniform float alphaColor;//alpha color of the fragment shader
uniform sampler2D screenTexture;

void main()
{
    //FragColor = texture(screenTexture,texCoords) * alphaColor;//austen graphics

    FragColor = texture(screenTexture,texCoords);//austen graphics

    FragColor = vec4(FragColor.x * alphaColor, FragColor.y * alphaColor , FragColor.z * alphaColor , FragColor.w);
}