#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;

uniform mat4 view_proj;

out vec2 frag_texcoord;

uniform sampler2D perlin_texture;

uniform float scale;
uniform float persistence;

void main()
{
	vec4 pos = position;
	pos.y += texture(perlin_texture, texcoord).r * scale;

	frag_texcoord = texcoord;
	gl_Position = view_proj * pos;
}