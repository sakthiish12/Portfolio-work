#shader vertex
#version 450 core

layout(location = 0) in vec4 position;

uniform mat4 u_MVP; // model view projection matrix , each letter is a 4 by 4 matrix which is a transformation 

void main()
{
	gl_Position = u_MVP * position;//getting the new position
}

#shader fragment
#version 450 core

layout(location = 0) out vec4 color;

uniform vec4 u_Color;
void main()
{
	color = u_Color;
}