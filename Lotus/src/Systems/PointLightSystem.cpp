#include "lotuspch.h"
#include "PointLightSystem.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace Lotus
{
    struct PointLightPushConstantData
	{
		glm::vec4 lightPos;
		glm::vec4 lightColor;
		float lightRadius;
	};

    PointLightSystem::PointLightSystem(Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        : m_Device{ device }
    {
        // default params
        CreatePipelineLayout(globalSetLayout);
        CreatePipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem()
    {
        vkDeviceWaitIdle(m_Device.GetDevice());
        vkDestroyPipelineLayout(m_Device.GetDevice(), m_PipelineLayout, nullptr);
    }

    void PointLightSystem::CreatePipelineLayout(VkDescriptorSetLayout globalSetLayout)
    {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstantData);

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

    void PointLightSystem::CreatePipeline(VkRenderPass renderPass)
    {
        assert(m_PipelineLayout != nullptr && "Cannot create pipeline before pipeline layout");

        PipelineConfigInfo pipelineConfig{};
        Pipeline::DefaultPipelineConfigInfo(pipelineConfig);
        Pipeline::EnableAlphaBlending(pipelineConfig);
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = m_PipelineLayout;
        m_Pipeline = std::make_unique<Pipeline>(
            m_Device,
            "../Lotus/Shaders/pointlightshader.vert.spv",
            "../Lotus/Shaders/pointlightshader.frag.spv",
            pipelineConfig
        );
    }

    void PointLightSystem::Update(FrameInfo& frameInfo, GlobalUbo& ubo)
    {
        auto rotate = glm::rotate(
            glm::mat4(1.0f),
            frameInfo.frameTime,
            glm::vec3{ 0.f, 0.f, 1.f }
        );
        int lightIndex = 0;
        for (auto& kv : frameInfo.gameObjects) 
        {
            auto &gameObject = kv.second;
            if (gameObject.pointLight)
            {
                assert(lightIndex < MAX_LIGHTS && "Too many pointlights!");
                // update light position
                gameObject.transform.position = rotate * glm::vec4(gameObject.transform.position, 1.0f);

                ubo.pointLights[lightIndex].position = glm::vec4(gameObject.transform.position, 1.0f);
				ubo.pointLights[lightIndex].color = glm::vec4(gameObject.color, gameObject.pointLight->lightIntensity);

                lightIndex++;
            }
        }
        ubo.numLights = lightIndex;
    }

    void PointLightSystem::Render(FrameInfo& frameInfo)
    {
        std::map<float, GameObject::id_t> sorted;
        for (auto& kv : frameInfo.gameObjects)
        {
			auto& gameObject = kv.second;
			if (gameObject.pointLight)
			{
				float distance = glm::length(frameInfo.camera.GetPosition() - gameObject.transform.position);
				sorted[distance] = kv.first;
			}
		 }

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

        for (auto it = sorted.rbegin(); it != sorted.rend(); it++)
        {
            auto& gameObject = frameInfo.gameObjects.at(it->second);
			if (gameObject.pointLight)
			{
				PointLightPushConstantData push{};
				push.lightPos = glm::vec4(gameObject.transform.position, 1.0f);
				push.lightColor = glm::vec4(gameObject.color, gameObject.pointLight->lightIntensity);
				push.lightRadius = gameObject.transform.scale.x;

				vkCmdPushConstants(
					frameInfo.commandBuffer,
					m_PipelineLayout,
					VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
					0,
					sizeof(PointLightPushConstantData),
					&push
				);

				vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
			}
        }

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
    }
}