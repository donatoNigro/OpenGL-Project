#include "Utility.h"
#include <cstdio>
#include "gl_core_4_4.h"

bool LoadShaderType(char* filename, GLenum shader_type, unsigned int* output)
{

	bool succeeded = true;

	FILE *shader_file = fopen(filename, "r");


	if (shader_file == 0)
	{
		succeeded = false;
	}
	else
	{
		fseek(shader_file, 0, SEEK_END);
		int shader_file_length = ftell(shader_file);
		fseek(shader_file, 0, SEEK_SET);

		char *shader_source = new char[shader_file_length];

		shader_file_length = fread(shader_source, 1, shader_file_length, shader_file);

		int success = GL_FALSE;
		int log_length = 0;

		unsigned int shader_handle = glCreateShader(shader_type);
		glShaderSource(shader_handle, 1, &shader_source, &shader_file_length);
		glCompileShader(shader_handle);

		glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &success);

		glGetShaderiv(shader_handle, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			int log_length = 0;
			glGetShaderiv(shader_handle, GL_INFO_LOG_LENGTH, &log_length);

			char* log = new char[log_length];
			glGetShaderInfoLog(shader_handle, log_length, 0, log);
			printf(filename);
			printf("THERES A ERROR !!!!!!!!!!!!!!!!!!\n\n");

			printf("%s", log);
			delete[] log;
			succeeded = false;
		}

		if (succeeded)
		{
			*output = shader_handle;

		}

		delete[]shader_source;
		fclose(shader_file);
	}
	return succeeded;

};


bool LoadShader(char* vertex_filename, char* geometry_filename, char* fragment_filename, unsigned int* result)
{
	bool succeeded = true;
	*result = glCreateProgram();
	unsigned int vertex_shader;
	unsigned int fragment_shader;
	unsigned int geometry_shader;

	LoadShaderType(vertex_filename, GL_VERTEX_SHADER, &vertex_shader);
	glAttachShader(*result, vertex_shader);
	glDeleteShader(vertex_shader);

	if (geometry_filename != nullptr)
	{
		LoadShaderType(geometry_filename, GL_GEOMETRY_SHADER, &geometry_shader);
		glAttachShader(*result, geometry_shader);
		glDeleteShader(geometry_shader);
	}

	if (fragment_filename != nullptr)
	{
		LoadShaderType(fragment_filename, GL_FRAGMENT_SHADER, &fragment_shader);
		glAttachShader(*result, fragment_shader);

		glDeleteShader(fragment_shader);
	}


	glLinkProgram(*result);

	GLint success;
	glGetProgramiv(*result, GL_LINK_STATUS, &success);

	if (success == GL_FALSE)
	{
		GLint log_length = 0;
		glGetProgramiv(*result, GL_INFO_LOG_LENGTH, &log_length);

		char* log = new char[log_length];
		glGetProgramInfoLog(*result, log_length, 0, log);

		printf("THERES A ERROR !!!!!!!!!!!!!!!!!!\n\n");
		printf("%s", log);
		delete[] log;
		succeeded = false;
	}

	glGetShaderiv(vertex_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int log_length = 0;
		glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);

		char* log = new char[log_length];
		glGetShaderInfoLog(vertex_shader, log_length, 0, log);

		printf("THERES A ERROR !!!!!!!!!!!!!!!!!!\n\n");
		printf("%s", log);
		delete[] log;
		succeeded = false;
	}

	glGetProgramiv(fragment_shader, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int log_length = 0;
		glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);

		char* log = new char[log_length];
		glGetShaderInfoLog(fragment_shader, log_length, 0, log);

		printf("THERES A ERROR !!!!!!!!!!!!!!!!!!\n\n");
		printf("%s", log);
		delete[] log;
		succeeded = false;
	}




	return succeeded;

}