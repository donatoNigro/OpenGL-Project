#ifndef _TEXTURING_H_
#define _TEXTURING_H_

#include "Application.h"
#include "FlyCamera.h"




class Texturing : public Application
{
public:

	virtual bool startup();
	virtual void shutdown();

	virtual bool update();
	virtual void draw();

	FlyCamera myCamera;

	unsigned int m_texture;
	unsigned int m_program;

	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;

	void loadTexture(const char* filename);
	void generateQuad(float size);



};
#endif
