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
        glm::mat4 modelMatrix{ 1.0f };
        glm::mat4 normalMatrix{ 1.0f };
        //alignas(16) glm::vec3 color;
    };

    SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : m_Device{ device }
    {
        // default params
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    Lotus::SimpleRenderSystem::SimpleRenderSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout, 
        VkPrimitiveTopology topology)
		: m_Device{ device }
    {
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass, topology);
    }


    SimpleRenderSystem::~SimpleRenderSystem()
    {
        vkDeviceWaitIdle(m_Device.GetDevice());
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    }

    void SimpleRenderSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = { globalSetLayout };

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
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

                vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            m_PipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr
        );

        for (auto& obj : gameObjects)
        {
            SimplePushConstantData push{};
            push.modelMatrix = obj.transform.GetTransform();
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