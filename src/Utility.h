#ifndef _UTILITY_H_
#define _UTILITY_H_

#pragma once
#include "glm_header.h"
#include "gl_core_4_4.h"

	bool LoadShader(char* vertex_filename,char * geometry_filename, char * fragment_filename, unsigned int* result);
	bool LoadShaderType(char* filename, GLenum shader_type, unsigned int* output);


#endif