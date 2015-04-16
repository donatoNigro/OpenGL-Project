#version 410

in vec2 frag_texcoord;
in vec4 frag_normal;
in vec4 frag_position;
in vec3 frag_tangent;
in vec3 frag_bitangent;

out vec4 frag_color;

uniform sampler2D perlin_texture;

uniform float scale;
uniform float persistence;

uniform float MIN;
uniform float MAX;


uniform float specular_power;
uniform vec3 ambient_light;
uniform vec3 light_dir;
uniform vec3 light_color;
uniform vec3 material_color;
uniform vec3 eye_pos;


uniform sampler2D diffuse_tex;
uniform sampler2D normal_tex;
uniform sampler2D specular_tex;


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
	perlin_sample -= MIN;
	perlin_sample /= (MAX - MIN);

	//perlin_sample * MIN;

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

	mat3 TBN = mat3(normalize(frag_tangent),
					normalize(frag_bitangent),
					normalize(frag_normal)
					);


	vec3 sampled_normal = texture(normal_tex, frag_texcoord).xyz;
	vec3 adjusted_normal = sampled_normal * 2 - 1;

	vec3 N = normalize(TBN * adjusted_normal);

	vec3 L = normalize(light_dir);


	vec3 material_color = texture(diffuse_tex, frag_texcoord).xyz;
	vec3 ambient = material_color * ambient_light;
	float d = max(0.0, dot(N, -L));
	vec3 diffuse = vec3(d) * light_color * material_color;
	vec3 E = normalize(eye_pos - frag_position.xyz);
	vec3 R = reflect(L, N);
	float S = max(0, dot(R, E));

	vec3 material_specular = texture(specular_tex, frag_texcoord).xyz;
	S = pow(S, specular_power);
	vec3 specular = vec3(S) * light_color * material_color;


	vec4 lighting = vec4(ambient + diffuse + specular, 1);

	frag_color *= lighting;

}