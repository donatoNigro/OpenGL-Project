#version 410

in vec2 frag_texcoord;
in vec3 frag_normal;
in vec4 frag_position;

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

uniform vec3 eye_pos;


uniform sampler2D diffuse_tex;
uniform sampler2D specular_tex;

void main()
{



	float perlin_sample = texture(perlin_texture, frag_texcoord).r;
	
	


	perlin_sample *= (persistence) * (scale / 10);
	perlin_sample -= MIN;
	perlin_sample /= (MAX - MIN);

	perlin_sample * MIN;

	//normals
	vec3 norm_pos = normalize(frag_position.xyz);
	vec3 dx = dFdx(frag_position.xyz);
	vec3 dy = dFdy(frag_position.xyz);
	normalize(dx);
	normalize(dy);
	vec3 n_vec = cross(dx, dy);
	//normals

	
	vec3 N = normalize(n_vec);
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


	frag_color = vec4(ambient + diffuse + specular, 1);

	
}