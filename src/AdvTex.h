#ifndef _ADV_TEX_H_
#define _ADV_TEX_H_
#include "Application.h"
#include "FlyCamera.h"
#include "Vertex.h"

class AdvTex : public Application
{
	public:

		bool startup();
		void shutdown();
		bool update();
		void draw();
		void generateQuad(float size);
		void loadTextures();




		FlyCamera myCamera;
		OpenGLData m_quad;

		unsigned int m_program_id;
		unsigned int m_diffuse_texture;
		unsigned int m_normal_texture;
		unsigned int m_specular_texture;

		vec3 m_ambient_light;
		vec3 m_light_dir;
		vec3 m_light_color;
	    float m_specular_power;


};


#endif