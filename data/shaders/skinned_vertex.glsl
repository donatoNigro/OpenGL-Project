#version 410

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 normal;
layout(location = 3) in vec4 bone_weights;
layout(location = 4) in vec4 bone_indices;  



out vec2 frag_tex_coord;

out vec4 frag_normal;
out vec4 frag_position;

uniform mat4 world;
uniform mat4 projection_view;

const int MAX_BONES = 128;
uniform mat4 bones[MAX_BONES];

void main()

{
	frag_tex_coord = tex_coord;

	ivec4 indices = ivec4(bone_indices);

	vec4 final_position = vec4(0, 0, 0, 0);

	final_position += bones[indices.x] * position * bone_weights.x;
	final_position += bones[indices.y] * position * bone_weights.y;
	final_position += bones[indices.z] * position * bone_weights.z;
	final_position.w = 1;

	frag_position = final_position;
	frag_normal = normal;

	gl_Position = projection_view * world * final_position;    
}