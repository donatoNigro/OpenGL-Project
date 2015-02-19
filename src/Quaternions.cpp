#include "Quaternions.h"
#include "FlyCamera.h"

mat4 EvaluateKeyframes(Keyframe start, Keyframe end, float t)
{

	vec3 pos = glm::mix(start.pos, end.pos, t);
	glm::quat rot = glm::mix(start.rot, end.rot, t);
	mat4 result = glm::translate(pos) * glm::toMat4(rot);

	return result;

}


bool Quaternions::startup()
{
	if (Application::startup() == false)
	{
		return false;

	}

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	Gizmos::create();

	myCamera.setSpeed(6.0f);
	myCamera.setSensitivity(2.0f);
	myCamera.setPosition(vec3(20, 20, 20));
	myCamera.setLookAt(vec3(10, 10, 10), vec3(0), vec3(0, 1, 0));
	//myCamera.setLookAt(vec3(-1, -1, -1), vec3(-1, -1, -1), vec3(-1, -1, -1));
	myCamera.setPerspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 1000.0f);



	m_hip_frames[0].pos = vec3(0, 5, 0);
	m_hip_frames[0].rot = glm::quat(vec3(-1, 0, 0));

	m_knee_frames[0].pos = vec3(0, -2.5, 0);
	m_knee_frames[0].rot = glm::quat(vec3(-1, 0, 0));

	m_ankle_frames[0].pos = vec3(0, -2.5, 0);
	m_ankle_frames[0].rot = glm::quat(vec3(0, 0, 0));

	m_hip_frames[1].pos = vec3(0, 5, 0);
    m_hip_frames[1].rot = glm::quat(vec3(1, 0, 0));
				   
	m_knee_frames[1].pos = vec3(0, -2.5, 0);
	m_knee_frames[1].rot = glm::quat(vec3(0, 0, 0));
				   
	m_ankle_frames[1].pos = vec3(0, -2.5, 0);
	m_ankle_frames[1].rot = glm::quat(vec3(-1, 0, 0));




	return true;
}

void Quaternions::shutdown()
{
	Gizmos::destroy();
	Application::shutdown();
}

bool Quaternions::update()
{
	if (Application::update() == false)
	{
		return false;
	}

	Gizmos::clear();

	float dt = (float)glfwGetTime();
	glfwSetTime(0.0);
	m_timer += dt;

	float sin_wave = sinf(m_timer) * 0.5f + 0.5f;

	m_hip_bone = EvaluateKeyframes(m_hip_frames[0], m_hip_frames[1], sin_wave);
	m_knee_bone = EvaluateKeyframes(m_knee_frames[0], m_knee_frames[1], sin_wave);
	m_ankle_bone = EvaluateKeyframes(m_ankle_frames[0], m_ankle_frames[1], sin_wave);

	mat4 global_hip = m_hip_bone;
	mat4 global_knee = m_hip_bone * m_knee_bone;
	mat4 global_ankle = global_knee * m_ankle_bone;
	
	vec3 hip_pos = global_hip[3].xyz;
	vec3 knee_pos = global_knee[3].xyz;
	vec3 ankle_pos = global_ankle[3].xyz;

	Gizmos::addAABBFilled(hip_pos, vec3(0.5f), vec4(1, 1, 0, 1), &global_hip);
	Gizmos::addAABBFilled(knee_pos, vec3(0.5f), vec4(1, 0, 0, 1), &global_knee);
	Gizmos::addAABBFilled(ankle_pos, vec3(0.5f), vec4(0, 1, 0, 1), &global_ankle);

	Gizmos::addLine(hip_pos, knee_pos, vec4(0, 1, 0, 1), vec4(1, 0, 1, 1));
	Gizmos::addLine(knee_pos, ankle_pos, vec4(0, 1, 0, 1), vec4(1, 0, 1, 1));



	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	

	vec4 white(1);

	vec4 black(0, 0, 0, 1);

	vec4 red(1, 0, 0, 1);

	vec4 green(0, 1, 0, 1);

	vec4 blue(0, 0, 1, 1);
	myCamera.update(dt, glfwGetCurrentContext());

	Gizmos::addTransform(mat4(1));
	for (int i = 0; i <= 20; ++i)
	{

		Gizmos::addLine(vec3(-10 + i, 0, -10), vec3(-10 + i, 0, 10),
			i == 10 ? white : black);
		Gizmos::addLine(vec3(-10, 0, -10 + i), vec3(10, 0, -10 + i),
			i == 10 ? white : black);
	}

	Gizmos::draw(myCamera.getProjection(), myCamera.getView());

	return true;
}




void Quaternions::draw()
{
	glfwSwapBuffers(glfwGetCurrentContext());
	glfwPollEvents();
}