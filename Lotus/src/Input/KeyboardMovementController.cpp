#include "lotuspch.h"
#include "KeyboardMovementController.h"

#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/constants.hpp"

namespace Lotus {

	void KeyboardMovementController::MoveInPlaneXZ(GLFWwindow* window, float deltaTime, GameObject& gameObject)
	{
        glm::vec3 rotate{ 0 };
        if (glfwGetKey(window, m_KeyMappings.lookRight) == GLFW_PRESS) rotate.y += 1.f;
        if (glfwGetKey(window, m_KeyMappings.lookLeft) == GLFW_PRESS) rotate.y -= 1.f;
        if (glfwGetKey(window, m_KeyMappings.lookUp) == GLFW_PRESS) rotate.x += 1.f;
        if (glfwGetKey(window, m_KeyMappings.lookDown) == GLFW_PRESS) rotate.x -= 1.f;

        if (glm::dot(rotate, rotate) > glm::epsilon<float>())
            gameObject.transform.rotation += m_RotationSpeed * deltaTime * glm::normalize(rotate);

        gameObject.transform.rotation.x = glm::clamp(gameObject.transform.rotation.x,
            -glm::half_pi<float>(), glm::half_pi<float>());
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());

        float yaw = gameObject.transform.rotation.y;
        glm::vec3 forward{ glm::sin(yaw), 0.f, glm::cos(yaw) };
        glm::vec3 right{ forward.z, 0.f, -forward.x };
        glm::vec3 up{ 0.f, -1.f, 0.f };

        glm::vec3 translation{ 0.f };
        if (glfwGetKey(window, m_KeyMappings.moveForward) == GLFW_PRESS) translation += forward;
        if (glfwGetKey(window, m_KeyMappings.moveBackward) == GLFW_PRESS) translation -= forward;
        if (glfwGetKey(window, m_KeyMappings.moveRight) == GLFW_PRESS) translation += right;
        if (glfwGetKey(window, m_KeyMappings.moveLeft) == GLFW_PRESS) translation -= right;
        if (glfwGetKey(window, m_KeyMappings.moveUp) == GLFW_PRESS) translation += up;
        if (glfwGetKey(window, m_KeyMappings.moveDown) == GLFW_PRESS) translation -= up;

        if (glm::dot(translation, translation) > glm::epsilon<float>())
        {
            gameObject.transform.translation =
                gameObject.transform.translation + m_MovementSpeed * deltaTime * glm::normalize(translation);
        }
	}

	void KeyboardMovementController::MoveInPlaneXY(GLFWwindow* window, float deltaTime, GameObject& gameObject)
	{
		if (glfwGetKey(window, m_KeyMappings.moveForward) == GLFW_PRESS) gameObject.transform.translation +=
			gameObject.transform.GetForwardVector() * m_MovementSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.moveBackward) == GLFW_PRESS) gameObject.transform.translation -=
			gameObject.transform.GetForwardVector() * m_MovementSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.moveRight) == GLFW_PRESS) gameObject.transform.translation +=
			gameObject.transform.GetRightVector() * m_MovementSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.moveLeft) == GLFW_PRESS) gameObject.transform.translation -=
			gameObject.transform.GetRightVector() * m_MovementSpeed * deltaTime;
		
		if (glfwGetKey(window, m_KeyMappings.moveUp) == GLFW_PRESS) gameObject.transform.translation +=
			glm::vec3{ 0.f, 0.f, 1.f } * m_MovementSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.moveDown) == GLFW_PRESS) gameObject.transform.translation -=
			glm::vec3{ 0.f, 0.f, 1.f } * m_MovementSpeed * deltaTime;

		if (glfwGetKey(window, m_KeyMappings.lookRight) == GLFW_PRESS) gameObject.transform.rotation.z += m_RotationSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.lookLeft) == GLFW_PRESS) gameObject.transform.rotation.z -= m_RotationSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.lookUp) == GLFW_PRESS) gameObject.transform.rotation.x -= m_RotationSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.lookDown) == GLFW_PRESS) gameObject.transform.rotation.x += m_RotationSpeed * deltaTime;
	}

}
