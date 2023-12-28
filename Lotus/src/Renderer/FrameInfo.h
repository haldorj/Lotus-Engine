#pragma once

#include "Camera/Camera.h"
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
    };
}