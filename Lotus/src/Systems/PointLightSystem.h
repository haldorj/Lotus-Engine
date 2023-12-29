#pragma once

#include "Window/Window.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Device.h"
#include "Lotus/GameObject.h"
#include "Camera/Camera.h"
#include "Renderer/FrameInfo.h"

#include <memory>

namespace Lotus
{
    class PointLightSystem
    {
    public:
        PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
        ~PointLightSystem();

        PointLightSystem(const PointLightSystem&) = delete; // delete copy constructor
        PointLightSystem operator=(const PointLightSystem&) = delete; // delete copy operator

        void Render(FrameInfo& frameInfo);
    private:
        void CreatePipelineLayout(VkDescriptorSetLayout globalDescriptorSet);
        void CreatePipeline(VkRenderPass renderPass);

    private:
        Device& m_Device;

        std::unique_ptr<Pipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
    };
}