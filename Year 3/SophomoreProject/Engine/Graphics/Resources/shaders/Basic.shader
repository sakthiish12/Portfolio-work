#shader vertex
#version 450 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

uniform mat4 u_MVP; // model view projection matrix , each letter is a 4 by 4 matrix which is a transformation 

void main()
{
	gl_Position = u_MVP * position; //calculating the gl position model view transfomrm * model position

	v_TexCoord = texCoord;  //take in from the vertex shader 
};


#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform vec4 u_Color; //uniform color
uniform sampler2D u_Texture;  //sample texture 

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoord); //calculating texture color 
	//color = vec4(1.0, 0.0, 0.0, 1.0); //hard coding color 
	color = texColor; //settint the texture color 
	//color = vec4(1.0);
};