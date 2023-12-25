#include "lotuspch.h"
#include "MouseMovementController.h"

namespace Lotus
{
    void MouseMovementController::UpdateMouse(GLFWwindow* window, float deltaTime, GameObject& gameObject) const
    {
        // Get the current mouse position
        double mouseX, mouseY;
        
        glfwGetCursorPos(window, &mouseX, &mouseY);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        // Calculate the difference in mouse position since the last frame
        static double lastMouseX = mouseX, lastMouseY = mouseY;
        const double deltaX = mouseX - lastMouseX;
        const double deltaY = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        // Calculate the new rotation for the GameObject
        const float rotationX = static_cast<float>(deltaTime * (deltaY * m_RotationSpeed));
        const float rotationZ = static_cast<float>(deltaTime * (deltaX * m_RotationSpeed));

        // Update the GameObject's rotation
        gameObject.transform.rotation.x += rotationX;
        gameObject.transform.rotation.z += rotationZ;
    }
}