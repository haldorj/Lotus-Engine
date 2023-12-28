#pragma once

#include "Window/Window.h"
#include "Pipeline.h"
#include "Device.h"
#include "Lotus/GameObject.h"
#include "Camera/Camera.h"

#include <memory>
#include "FrameInfo.h"

namespace Lotus
{
    class SimpleRenderSystem
    {
    public:
        SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, VkPrimitiveTopology topology);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete; // delete copy constructor
        SimpleRenderSystem operator=(const SimpleRenderSystem&) = delete; // delete copy operator

        void RenderGameObjects(FrameInfo& frameInfo);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet);
        void CreatePipeline(VkRenderPass renderPass, VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

    private:
        Device& m_Device;

        std::unique_ptr<Pipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
    };
}