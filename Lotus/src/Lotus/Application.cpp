#include "lotuspch.h"
#include "Application.h"

#include "Lotus/Log.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Lotus {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application::Application()
    {
        LoadModels();
        CreatePipelineLayout();
        CreatePipeline();
        CreateCommandBuffers();
    }

    Application::~Application()
    {
        vkDeviceWaitIdle(m_Device.GetDevice());
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    }

    void Application::Run()
    {
        while (!m_Window.Closed())
        {
            m_Window.Update();
            DrawFrame();
        }
    }

    void Application::LoadModels()
    {
        std::vector<Model::Vertex> vertices{ 
                {{0.0f, -0.5f}}, {{0.5f, 0.5f}}, {{-0.5f, 0.5f}} 
        };
        m_Model = std::make_unique<Model>(m_Device, vertices);
    }

    void Application::CreatePipelineLayout()
    {
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        {
            LOTUS_CORE_ERROR("failed to create pipeline layout!");
        }
    }

    void Application::CreatePipeline()
    {
        PipelineConfigInfo pipelineConfig{};
        Pipeline::DefaultPipelineConfigInfo(
            pipelineConfig,
            m_SwapChain.Width(),
            m_SwapChain.Height()
        );
        pipelineConfig.renderPass = m_SwapChain.GetRenderPass();
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<Pipeline>(
            m_Device,
            "../Lotus/Shaders/simpleshader.vert.spv",
            "../Lotus/Shaders/simpleshader.frag.spv",
            pipelineConfig
        );
    }

    void Application::CreateCommandBuffers()
    {
        m_CommandBuffers.resize(m_SwapChain.ImageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_Device.GetCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());
        if (vkAllocateCommandBuffers(m_Device.GetDevice(), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS)
        {
            LOTUS_CORE_ERROR("Failed to allocate command buffers");
        }
        for (int i = 0; i < m_CommandBuffers.size(); i++)
        {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo) != VK_SUCCESS)
            {
                LOTUS_CORE_ERROR("Failed to begin recording command buffer");
            }
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = m_SwapChain.GetRenderPass();
            renderPassInfo.framebuffer = m_SwapChain.GetFrameBuffer(i);

            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = m_SwapChain.GetSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { {0.2f, 0.01f, 0.4f, 1.0f} };
            clearValues[1].depthStencil = { 1.0f, 0 };
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            m_Pipeline->Bind(m_CommandBuffers[i]);
            m_Model->Bind(m_CommandBuffers[i]);
            m_Model->Draw(m_CommandBuffers[i]);

            vkCmdEndRenderPass(m_CommandBuffers[i]);
            if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS)
            {
                LOTUS_CORE_ERROR("Failed to record command buffer");
            }
        }
    }

    void Application::DrawFrame()
    {
        uint32_t imageIndex;
        auto result = m_SwapChain.AcquireNextImage(&imageIndex);
        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
        {
            LOTUS_CORE_ERROR("Failed to acquire swap chain image");
        }
        result = m_SwapChain.SubmitCommandBuffers(&m_CommandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS)
        {
            LOTUS_CORE_ERROR("Failed to present swap chain image");
        }
    }

}