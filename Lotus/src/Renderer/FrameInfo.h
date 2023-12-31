#pragma once

#include "Camera/Camera.h"
#include "GameObject/GameObject.h"
#include <vulkan/vulkan.h>

namespace Lotus
{
    #define MAX_LIGHTS 10
    struct PointLight
    {
        glm::vec4 position{};
        glm::vec4 color{};
    };

    struct GlobalUbo
    {
        glm::mat4 projection{ 1.f };
        glm::mat4 view{ 1.f };
        glm::mat4 inverseView{ 1.f };
        glm::vec4 ambientLight{ 1.0f, 1.0f, 1.0f, 0.02f };
        PointLight pointLights[MAX_LIGHTS]{};
        int numLights;
    };

    struct FrameInfo
    {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
        VkDescriptorSet globalDescriptorSet;
        GameObject::Map& gameObjects;
    };
}