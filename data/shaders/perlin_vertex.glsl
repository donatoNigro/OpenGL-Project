#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 texcoord;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec4 tangent;

uniform mat4 view_proj;

out vec2 frag_texcoord;

out vec4 frag_normal;
out vec4 frag_position;
out vec3 frag_tangent;
out vec3 frag_bitangent;


uniform sampler2D perlin_texture;

uniform float scale;
uniform float persistence;

void main()
{
	vec4 pos = position;
	pos.y += texture(perlin_texture, texcoord).r * scale;

	frag_texcoord = texcoord;
	frag_position = pos;
	frag_normal = normal;
	frag_tangent = tangent.xyz;
	frag_bitangent = cross(normal.xyz, tangent.xyz);


	gl_Position = view_proj * pos;
}