#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 normal;

uniform mat4 view_proj;

out vec2 frag_texcoord;
out vec4 frag_position;
out vec4 frag_normal;


uniform sampler2D perlin_texture;

uniform float scale;
uniform float persistence;



void main()
{
	vec4 pos = position;
	pos.y += texture(perlin_texture, texcoord).r * scale;

	//vec3 v1 = vec3(1, texture(perlin_texture, texcoord),0);
	//vec3 v2;
	//vec3 n = cross(v1 - position, v2 - position);
	//n = normalize(n);

	frag_texcoord = texcoord;
	frag_position = pos;
	frag_normal = normal;



	gl_Position = view_proj * pos;
}