#include "lotuspch.h"
#include "KeyboardMovementController.h"

#include "glm/ext/scalar_constants.hpp"
#include "glm/gtc/constants.hpp"

namespace Lotus {

	void KeyboardMovementController::MoveInPlaneXY(GLFWwindow* window, float deltaTime, GameObject& gameObject)
	{
		if (glfwGetKey(window, m_KeyMappings.moveForward) == GLFW_PRESS) gameObject.transform.position +=
			gameObject.transform.GetForwardVector() * m_MovementSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.moveBackward) == GLFW_PRESS) gameObject.transform.position -=
			gameObject.transform.GetForwardVector() * m_MovementSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.moveRight) == GLFW_PRESS) gameObject.transform.position +=
			gameObject.transform.GetRightVector() * m_MovementSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.moveLeft) == GLFW_PRESS) gameObject.transform.position -=
			gameObject.transform.GetRightVector() * m_MovementSpeed * deltaTime;
		
		if (glfwGetKey(window, m_KeyMappings.moveUp) == GLFW_PRESS) gameObject.transform.position +=
			glm::vec3{ 0.f, 0.f, 1.f } * m_MovementSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.moveDown) == GLFW_PRESS) gameObject.transform.position -=
			glm::vec3{ 0.f, 0.f, 1.f } * m_MovementSpeed * deltaTime;

		if (glfwGetKey(window, m_KeyMappings.lookRight) == GLFW_PRESS) gameObject.transform.rotation.z += m_RotationSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.lookLeft) == GLFW_PRESS) gameObject.transform.rotation.z -= m_RotationSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.lookUp) == GLFW_PRESS) gameObject.transform.rotation.x -= m_RotationSpeed * deltaTime;
		if (glfwGetKey(window, m_KeyMappings.lookDown) == GLFW_PRESS) gameObject.transform.rotation.x += m_RotationSpeed * deltaTime;
	}

}
