#include "lotuspch.h"
#include "SimpleRenderSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Lotus
{
    struct SimplePushConstantData
    {
        glm::mat4 transform{ 1.0f };
        glm::mat4 normalMatrix{ 1.0f };
        //alignas(16) glm::vec3 color;
    };

    SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass)
        : m_Device{ device }
    {
        // default params
        CreatePipelineLayout();
        CreatePipeline(renderPass);
    }

    Lotus::SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkPrimitiveTopology topology)
		: m_Device{ device }
    {
        CreatePipelineLayout();
        CreatePipeline(renderPass, topology);
    }


    SimpleRenderSystem::~SimpleRenderSystem()
    {
        vkDeviceWaitIdle(m_Device.GetDevice());
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    }

    void SimpleRenderSystem::CreatePipelineLayout()
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(m_Device.GetDevice(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void SimpleRenderSystem::CreatePipeline(VkRenderPass renderPass, 
        VkPrimitiveTopology topology)
    {
        assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::DefaultPipelineConfigInfo(
            pipelineConfig
        );

        pipelineConfig.inputAssemblyInfo.topology = topology;

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<Pipeline>(
            m_Device,
            "../Lotus/Shaders/simpleshader.vert.spv",
            "../Lotus/Shaders/simpleshader.frag.spv",
            pipelineConfig
        );
    }

    void SimpleRenderSystem::RenderGameObjects(FrameInfo& frameInfo, std::vector<GameObject>& gameObjects)
    {
        m_Pipeline->Bind(frameInfo.commandBuffer);

        const auto projectionView = frameInfo.camera.GetProjectionMatrix() * frameInfo.camera.GetViewMatrix();

        for (auto& obj : gameObjects)
        {
            SimplePushConstantData push{};
            auto modelMatrix = obj.transform.GetTransform();
            push.transform = projectionView * modelMatrix;
            push.normalMatrix = obj.transform.GetNormalMatrix();

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                m_PipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );
            obj.model->Bind(frameInfo.commandBuffer);
            obj.model->Draw(frameInfo.commandBuffer);
        }
    }
}