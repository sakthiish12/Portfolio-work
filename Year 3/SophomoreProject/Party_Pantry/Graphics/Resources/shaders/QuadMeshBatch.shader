#shader vertex
#version 450 core

layout(location = 0) in vec4 position;//position
layout(location = 1) in vec4 a_Color;//color 

out vec4 v_Color;

uniform mat4 u_MVP; // model view projection matrix , each letter is a 4 by 4 matrix which is a transformation 

void main()
{
	gl_Position = u_MVP * position; //calculating the gl position model view transfomrm * model position

	v_Color = a_Color;//passing into the color 
};


#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

in vec4 v_Color;//getting the color 

void main()
{
	color = v_Color; //settint the texture color 

};