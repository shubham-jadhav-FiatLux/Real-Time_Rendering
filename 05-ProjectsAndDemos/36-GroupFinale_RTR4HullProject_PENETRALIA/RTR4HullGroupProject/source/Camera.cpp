#include "Camera.h"

#define YAW -90.0f
#define PITCH 0.0f
#define SPEED 4.1f
#define SENSITIVITY 0.6f
#define ZOOM 45.0f

float yaw = YAW;
float pitch = PITCH;
float movement_speed = SPEED;
float mouse_sensitivity = SENSITIVITY;
//float zoom = ZOOM;

//bool firstMouse = true;
bool constraintPitch = true;


vmath::vec3 cameraPosition = vec3(1340.0f, 65.0f, -850.0f);
vmath::vec3 cameraFront;
vmath::vec3 cameraUp = vec3(0.0f, 1.0f, 0.0f);
vmath::vec3 cameraRight;
vmath::vec3 cameraWorldUp = cameraUp;

void InitializeCamera(void)
{
	UpdateCamera();
}

void UpdateCamera()
{
	vmath::vec3 front;
	front[0] = (float)cos(vmath::radians(yaw)) * (float)cos(vmath::radians(pitch));
	front[1] = (float)sin(vmath::radians(pitch));
	front[2] = (float)sin(vmath::radians(yaw)) * (float)cos(vmath::radians(pitch));
	cameraFront = front;
	cameraRight = vmath::normalize(vmath::cross(cameraFront, cameraWorldUp));
	cameraUp = vmath::normalize(vmath::cross(cameraRight, cameraFront));
}

void CameraMoveForward(float frameTime)
{
	cameraPosition -= movement_speed * cameraFront;
}

void CameraMoveBackward(float frameTime)
{
	cameraPosition += movement_speed * cameraFront;
}

void CameraMoveRight(float frameTime)
{
	cameraPosition += cameraRight * movement_speed;
}

void CameraMoveLeft(float frameTime)
{
	cameraPosition -= cameraRight * movement_speed;
}

void CameraMoveUp(float frameTime)
{
	cameraPosition += cameraUp * movement_speed;
}

void CameraMoveDown(float frameTime)
{
	cameraPosition -= cameraUp * movement_speed;
}

void CameraMoveOnMouse(float mouseXpos,float mouseYpos, float lastX, float lastY)
{
	float xoffset = mouseXpos - lastX;
	float yoffset = lastY - mouseYpos;


	xoffset *= mouse_sensitivity;
	yoffset *= mouse_sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (constraintPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	vmath::vec3 front;
	front[0] = (float)cos(vmath::radians(yaw)) * (float)cos(vmath::radians(pitch));
	front[1] = (float)sin(vmath::radians(pitch));
	front[2] = (float)sin(vmath::radians(yaw)) * (float)cos(vmath::radians(pitch));
	cameraFront = vmath::normalize(front);
	cameraRight = vmath::normalize(vmath::cross(cameraFront, cameraWorldUp));
	cameraUp = vmath::normalize(vmath::cross(cameraRight, cameraFront));
}
mat4 GetViewMatrix()
{
	return vmath::lookat(cameraPosition, cameraPosition + cameraFront, cameraUp);
}