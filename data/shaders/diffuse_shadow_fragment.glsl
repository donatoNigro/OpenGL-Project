#version 410

in vec4 frag_normal;

uniform vec3 light_dir;

out vec4 frag_color;

in vec4 shadow_coord;
uniform sampler2D shadow_map;



void main()
{

	float d = max(0, dot(-light_dir, normalize(frag_normal).xyz));
	float distance_from_light = texture(shadow_map, shadow_coord.xy).r;

	if( distance_from_light < shadow_coord.z - 0.01f)
	{
		d = 0; 
	}

	frag_color = vec4(d,d,d,1);

}
