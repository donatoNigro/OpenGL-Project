#version 410

in vec2 frag_texcoord;

out vec4 frag_color;

uniform float offset;

uniform sampler2D input_texture;

vec4 BoxBlur()
{
	vec2 texel_uv = 1.0f / textureSize(input_texture, 0).xy;

	vec4 color = texture(input_texture, frag_texcoord);

	color += texture(input_texture, frag_texcoord + texel_uv);

	color += texture(input_texture, frag_texcoord + vec2(texel_uv.x, 0));
	color += texture(input_texture, frag_texcoord + vec2(texel_uv.x, -texel_uv.y));
	color += texture(input_texture, frag_texcoord + vec2(0, texel_uv.y));
	color += texture(input_texture, frag_texcoord + vec2(0, -texel_uv.y));
	color += texture(input_texture, frag_texcoord + vec2(-texel_uv.x, -texel_uv.y));
	color += texture(input_texture, frag_texcoord + vec2(-texel_uv.x, 0));
	color += texture(input_texture, frag_texcoord + vec2(-texel_uv.x, -texel_uv.y));
	
	color /= 9;

	return color;

}

vec4 StaticWave()
{
	vec4 color;
	vec2 tex_coord = frag_texcoord;
	tex_coord.x += sin(tex_coord.y * 4*2*3.14159 + offset) / 100;
	color = texture2D(input_texture, tex_coord);

	float trash = sin(4*2*3.14159 + offset) / 100;

	color *= vec4(trash, 1  , trash, 1);

	return color;


}

void main()
{

	frag_color = StaticWave();

}

