#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lotus
{

	class Camera
	{
	public:
		void SetOrthographicProjection(float left, float right, float top, float bottom, float near, float far);
		void SetPerspectiveProjection(float fovy, float aspect, float near, float far);

		void SetViewDirection(const glm::vec3 position, const glm::vec3 direction, const glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
		void SetViewTarget(const glm::vec3 position, const glm::vec3 target, const glm::vec3 up = glm::vec3{ 0.f, -1.f, 0.f });
		void SetViewYXZ(const glm::vec3 position, const glm::vec3 rotation);

		const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
	private:
		glm::mat4 m_ProjectionMatrix{ 1.f };
		glm::mat4 m_ViewMatrix{ 1.f };
	};

}