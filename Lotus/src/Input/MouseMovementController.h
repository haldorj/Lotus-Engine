#pragma once

#include "GameObject/GameObject.h"
#include "Window/Window.h"

namespace  Lotus
{
    class MouseMovementController
    {
    public:
        void UpdateMouse(GLFWwindow* window, float deltaTime, GameObject& gameObject) const;

    private:
        float m_RotationSpeed = 1.f;
    };
}