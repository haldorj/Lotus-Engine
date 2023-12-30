#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lotus
{

	class Camera
	{
	public:
		Camera();
		void SetOrthographicProjection(float left, float right, float top, float bottom);
		void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

		void LookAt(glm::vec3 eye, glm::vec3& target, glm::vec3& up);
		
		const glm::vec3& GetPosition() const { return m_Position; }
		void SetPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }
		
		const glm::vec3 GetRotation() const { return m_Rotation; }
		void SetRotation(glm::vec3 rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& GetInverseViewMatrix() const { return m_InverseViewMatrix; }
		const glm::mat4& GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
	private:
		void RecalculateViewMatrix();
	private:
		glm::mat4 m_ProjectionMatrix = {};
		glm::mat4 m_ViewMatrix = {};
		glm::mat4 m_InverseViewMatrix = {};
		glm::mat4 m_ViewProjectionMatrix = {};
		
		glm::vec3 m_Position = {};
		glm::vec3 m_Rotation = {};
	};

}