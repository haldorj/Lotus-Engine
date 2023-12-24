#include "lotuspch.h"
#include "Camera.h"

#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

namespace Lotus {
	Camera::Camera()
	{
		m_ProjectionMatrix = glm::mat4{1.0f};
		m_ViewMatrix = glm::mat4{1.0f};
	}

	void Camera::SetOrthographicProjection(float left, float right, float top, float bottom)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void Camera::SetPerspectiveProjection(float fovy, float aspect, float nearPlane, float farPlane)
	{
		m_ProjectionMatrix = glm::perspective(fovy, aspect, nearPlane, farPlane);
		m_ProjectionMatrix[1][1] *= -1;
	}

	void Camera::LookAt(glm::vec3 eye, glm::vec3& target, glm::vec3& up)
	{
		m_ViewMatrix = glm::lookAt(eye, target, up);
		m_Position = eye;
	}
	
	void Camera::RecalculateViewMatrix()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) *
			glm::rotate(glm::mat4(1.0f), m_Rotation.x, glm::vec3(1, 0, 0)) *
			glm::rotate(glm::mat4(1.0f), m_Rotation.y, glm::vec3(0, 1, 0)) *
			glm::rotate(glm::mat4(1.0f), m_Rotation.z, glm::vec3(0, 0, 1));
		
		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	
	

}
