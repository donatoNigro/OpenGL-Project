#version 410
in vec4 position; 
in vec4 color; 

out vec4 out_color; 

uniform mat4 projection_view; 

void main() 
{ 
	out_color = color; 
	gl_Position = projection_view * position;
}