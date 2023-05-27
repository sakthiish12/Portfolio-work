#shader vertex
#version 450 core

layout(location = 0) in vec4 position;//position
layout(location = 1) in vec4 v_Color;//color

out vec4 f_Color;//set out color 

uniform mat4 u_MVP; // model view projection matrix , each letter is a 4 by 4 matrix which is a transformation 

void main()
{
	gl_Position = u_MVP * position;//getting the new position
	f_Color = v_Color;//set the color
};

#shader fragment
#version 450 core

in vec4 f_Color;

layout(location = 0) out vec4 color;

void main()
{
	color = f_Color;
};