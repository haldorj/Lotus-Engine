#pragma once

#include "Lotus/GameObject.h"
#include "Window/Window.h"

namespace  Lotus
{
    class KeyboardMovementController
    {
    public:
        struct KeyMappings
        {
            int moveLeft = GLFW_KEY_A;
            int moveRight = GLFW_KEY_D;
            int moveForward = GLFW_KEY_W;
            int moveBackward = GLFW_KEY_S;
            int moveUp = GLFW_KEY_SPACE;
            int moveDown = GLFW_KEY_LEFT_CONTROL;
            int lookLeft = GLFW_KEY_LEFT;
            int lookRight = GLFW_KEY_RIGHT;
            int lookUp = GLFW_KEY_UP;
            int lookDown = GLFW_KEY_DOWN;
        };

        void MoveInPlaneXY(GLFWwindow* window, float deltaTime, GameObject& gameObject);

        KeyMappings m_KeyMappings;
        float m_MovementSpeed = 3.0f;
        float m_RotationSpeed = 90.f;
    };
}