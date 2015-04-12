#version 410

in vec2 frag_texcoord;

out vec4 frag_color;

uniform sampler2D perlin_texture;

uniform float scale;
uniform float persistence;

uniform float min;
uniform float max;

void main()
{
	
	vec4 red = vec4(1, 0, 0, 1);
	vec4 green = vec4(0, 1, 0, 1);
	vec4 blue = vec4(0, 0, 1, 1);

	vec4 white = vec4(1, 1, 1, 1);
	vec4 black = vec4(0, 0, 0, 1);

	vec4 brown = vec4(1.0f * (90.0f/255.0f), 1.0f * (51.0f/255.0f), 0, 1);


	float perlin_sample = texture(perlin_texture, frag_texcoord).r;
	
	//perlin_sample *= (persistence) * (scale / 10);
	perlin_sample -= min;
	perlin_sample /= (max - min);

	//perlin_sample * min;

	if(perlin_sample > 0.49)
	{
		if(perlin_sample > 0.6)
		{
			if(perlin_sample > 0.75)
			{
				if(perlin_sample > 1)
				{
					frag_color = mix(vec4(perlin_sample), white, 0.25);
				}
				else
					frag_color = mix(vec4(perlin_sample), mix(black, white, 0.875), 0.25);
			}
			else
				frag_color = mix(
					vec4(perlin_sample),
					mix(black, white, 0.5),
					0.25);
		}
		else
			frag_color = mix(
				vec4(perlin_sample),
				mix(green, black, 0.6),
				0.25);
	}
	else
	{
		if(perlin_sample > 0.2 * perlin_sample)
		{
			frag_color = mix(
				vec4(perlin_sample),
				mix(brown, black, 0.3),
				0.25);
		}
		else
			frag_color = mix(vec4(perlin_sample), black, 0.25);
	}

	//frag_color = vec4(perlin_sample);
	frag_color.a = 1;
}