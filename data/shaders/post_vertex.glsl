#version 410

layout (location=0) in vec4 position;
layout (location=1) in vec2 tex_coord;

out vec2 frag_texcoord;


void main()
{	
	gl_Position = position;
	frag_texcoord = tex_coord;
}