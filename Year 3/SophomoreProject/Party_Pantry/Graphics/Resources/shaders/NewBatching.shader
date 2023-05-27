#shader vertex
#version 450 core

layout(location = 0) in vec4 position;//position
layout(location = 1) in vec4 a_Color;//color 
layout(location = 2) in vec2 texCoord;//texture coordinates 
layout(location = 3) in float a_texIndex;//texture index 

out vec2 v_TexCoord;
out float v_texIndex;
out vec4 v_Color;

uniform mat4 u_MVP; // model view projection matrix , each letter is a 4 by 4 matrix which is a transformation 

void main()
{
	gl_Position = u_MVP * position; //calculating the gl position model view transfomrm * model position

	v_TexCoord = texCoord;  //take in from the vertex shader 

	v_texIndex = a_texIndex; //take in the texture index 

	v_Color = a_Color;//passing into the color 
};


#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in float v_texIndex;//taking in the texture unit 
in vec4 v_Color;//getting the color 

uniform sampler2D u_Texture[32];  //sample texture 

void main()
{
	int index = int(v_texIndex);//passing in the index of the texture
	vec4 texColor = texture(u_Texture[index], v_TexCoord); //getting the texcolor based on the texture index
	if(texColor.a < 0.1)
	{
		discard;
	}
	color = texColor * vec4(v_Color); //settint the texture color 

};