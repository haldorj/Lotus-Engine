#pragma once

#include "Camera/Camera.h"
#include "Lotus/GameObject.h"
#include <vulkan/vulkan.h>

namespace Lotus
{
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