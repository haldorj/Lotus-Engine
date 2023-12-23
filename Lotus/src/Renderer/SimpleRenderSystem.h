#pragma once

#include "Window/Window.h"
#include "Pipeline.h"
#include "Device.h"
#include "Lotus/GameObject.h"
#include "Camera/Camera.h"

#include <memory>

namespace Lotus
{
    class SimpleRenderSystem
    {
    public:
        SimpleRenderSystem(Device& device, VkRenderPass renderPass);
        ~SimpleRenderSystem();

        SimpleRenderSystem(const SimpleRenderSystem&) = delete; // delete copy constructor
        SimpleRenderSystem operator=(const SimpleRenderSystem&) = delete; // delete copy operator

        void RenderGameObjects(VkCommandBuffer commandBuffer, std::vector<GameObject>& gameObjects, const Camera& camera);
    private:
        void CreatePipelineLayout();
        void CreatePipeline(VkRenderPass renderPass);

    private:
        Device& m_Device;

        std::unique_ptr<Pipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
    };
}