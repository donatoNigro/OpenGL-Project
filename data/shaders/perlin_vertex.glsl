#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 normal;

uniform mat4 view_proj;

out vec2 frag_texcoord;
out vec4 frag_position;
out vec3 frag_normal;


uniform sampler2D perlin_texture;

uniform float scale;
uniform float persistence;



void main()
{

	vec4 pos = position;
	pos.y += texture(perlin_texture, texcoord).r * scale;





	frag_texcoord = texcoord;
	frag_position = pos;
	//frag_normal = n_vec;

	frag_normal = normal.xyz;

	gl_Position = view_proj * pos;
}