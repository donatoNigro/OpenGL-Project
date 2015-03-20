#include "GPUEmitter.h"

GPUPointEmitter::GPUPointEmitter():m_particles(nullptr), m_max_particles(0), m_position(0), m_draw_shader(0), m_update_shader(0), m_last_draw_time(0)
{
	m_vao[0] = 0;
	m_vao[1] = 0;
	m_vbo[0] = 0;
	m_vbo[1] = 0;
}

GPUPointEmitter::~GPUPointEmitter()
{
	delete[] m_particles; 
	glDeleteVertexArrays(2, m_vao);
	glDeleteBuffers(2, m_vbo);

	glDeleteProgram(m_update_shader);
	glDeleteProgram(m_draw_shader);

}

void GPUPointEmitter::Init(unsigned int a_max_particles, vec3 a_position, float a_emit_rate, float a_min_lifespan, float a_max_lifespan,
	 float a_velocity_min, float a_velocity_max, float a_start_size, float a_end_size, vec4 a_start_color, vec4 a_end_color)
{
	m_position = a_position;
	m_start_color = a_start_color;
	m_end_color = a_end_color;
	m_start_size = a_start_size;
	m_end_size = a_end_size;
	m_velocity_min = a_velocity_min;
	m_velocity_max = a_velocity_max;

	m_lifespan_min = a_min_lifespan;
	m_lifespan_max = a_max_lifespan;

	m_max_particles = a_max_particles;

	m_particles = new GPUParticle[m_max_particles];
	m_active_buffer = 0;

	CreateBuffers();
	CreateUpdateShader();
	CreateDrawShader();



}

void GPUPointEmitter::Draw(float a_curr_time, mat4 a_camera_transform, mat4 a_projection_view)
{
	//update vertex pass
	glUseProgram(m_update_shader);

	unsigned int delta_uniform = glGetUniformLocation(m_update_shader, "delta_time");
	unsigned int emitter_pos_uniform = glGetUniformLocation(m_update_shader, "emitter_position");

	int min_vel_uniform = glGetUniformLocation(m_update_shader, "min_velocity");
	int max_vel_uniform = glGetUniformLocation(m_update_shader, "max_velocity");
	int min_lifespan_uniform = glGetUniformLocation(m_update_shader, "min_lifespan");
	int max_lifespan_uniform = glGetUniformLocation(m_update_shader, "max_lifespan");
	int time_uniform = glGetUniformLocation(m_update_shader, "time");
	
	glUniform1f(delta_uniform, a_curr_time - m_last_draw_time);
	glUniform3fv(emitter_pos_uniform, 1, (float*)&m_position);

	glUniform1f(min_vel_uniform, m_velocity_min);
	glUniform1f(max_vel_uniform, m_velocity_max);

	glUniform1f(min_lifespan_uniform, m_lifespan_min);
	glUniform1f(max_lifespan_uniform, m_lifespan_max);

	glUniform1f(time_uniform, a_curr_time);

	glEnable(GL_RASTERIZER_DISCARD);

	glBindVertexArray(m_vao[m_active_buffer]);

	unsigned int other_buffer = (m_active_buffer + 1) % 2;

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, m_vbo[other_buffer]);

	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, m_max_particles); 

	glEndTransformFeedback();
	glDisable(GL_RASTERIZER_DISCARD);

	glBindBufferBase(GL_TRANSFORM_FEEDBACK, 0, 0);

	glUseProgram(m_draw_shader);

	int proj_view_uniform = glGetUniformLocation(m_draw_shader, "projection_view");
	int camera_world_uniform = glGetUniformLocation(m_draw_shader, "camera_world");
	int start_size_uniform = glGetUniformLocation(m_draw_shader, "start_size");
	int end_size_uniform = glGetUniformLocation(m_draw_shader, "end_size");
	int start_color_uniform = glGetUniformLocation(m_draw_shader, "start_color");
	int end_color_uniform = glGetUniformLocation(m_draw_shader, "end_color");

	glUniformMatrix4fv(proj_view_uniform, 1, GL_FALSE, (float*)&a_projection_view);
	glUniformMatrix4fv(camera_world_uniform, 1, GL_FALSE, (float*)&a_camera_transform);
	
	glUniform1f(start_size_uniform, m_start_size);
	glUniform1f(end_size_uniform, m_end_size);
	glUniform4fv(start_color_uniform, 1, (float*)&m_start_color);
	glUniform4fv(end_color_uniform, 1, (float*)&m_end_color);

	glBindVertexArray(m_vao[other_buffer]);
	glDrawArrays(GL_POINTS, 0, m_max_particles);

	m_active_buffer = other_buffer;


	m_last_draw_time = a_curr_time;




}

void GPUPointEmitter::CreateBuffers()
{
	glGenVertexArrays(2, m_vao);
	glGenBuffers(2, m_vbo);

	for (unsigned int buffer_index = 0; buffer_index < 2; ++buffer_index)
	{
		glBindVertexArray(m_vao[buffer_index]);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[buffer_index]);
		glBufferData(GL_ARRAY_BUFFER, m_max_particles * sizeof(GPUParticle), m_particles, GL_STREAM_DRAW);
		
		glEnableVertexAttribArray(0); //pos
		glEnableVertexAttribArray(1); //velocity
		glEnableVertexAttribArray(2); //lifetime
		glEnableVertexAttribArray(3); //lifespan

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(12));
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(24));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(GPUParticle), (void*)(28));

	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void GPUPointEmitter::CreateUpdateShader()
{
	unsigned int vertex_shader;
	LoadShaderType("./data/shaders/particle_update_vert.glsl", GL_VERTEX_SHADER, &vertex_shader);
	m_update_shader = glCreateProgram();
	glAttachShader(m_update_shader, vertex_shader);

	const char* outputs[] = { "updated_position", "updated_velocity", "updated_lifetime", "updated_lifespan" };

	glTransformFeedbackVaryings(m_update_shader, 4, outputs, GL_INTERLEAVED_ATTRIBS);

	glLinkProgram(m_update_shader);
	glDeleteShader(vertex_shader);

}

void GPUPointEmitter::CreateDrawShader()
{
	LoadShader("./data/shaders/gpu_particle_vert.glsl", "./data/shaders/gpu_particle_geom.glsl", "./data/shaders/gpu_particle_frag.glsl", &m_draw_shader);


}
