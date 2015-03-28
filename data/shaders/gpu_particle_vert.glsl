#version 410

layout(Location=0) in vec3 position;
layout(Location=1) in vec3 velocity;
layout(Location=2) in float lifetime;
layout(Location=3) in float lifespan;

out vec3 geo_position;
out float geo_lifespan;
out float geo_lifetime;

void main()
{
	geo_position = position;
	geo_lifespan = lifespan;
	geo_lifetime = lifetime;

}