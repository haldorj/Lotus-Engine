#include "lotuspch.h"
#include "Application.h"
#include "Systems/SimpleRenderSystem.h"
#include "Systems/PointLightSystem.h"
#include "Camera/Camera.h"
#include "Input/KeyboardMovementController.h"
#include "Input/MouseMovementController.h"
#include "Renderer/Buffer.h"

#include "Lotus/Log.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "tiny_obj_loader.h"
#include <Renderer/FrameInfo.h>

namespace Lotus {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

    Application::Application()
    {
        m_GlobalPool =
            DescriptorPool::Builder(m_Device)
            .SetMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .AddPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .Build();

        LoadGameObjects();
    }

    Application::~Application()
    {
        vkDeviceWaitIdle(m_Device.GetDevice());
    }

    void Application::Run()
    {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<Buffer>(
                m_Device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
            uboBuffers[i]->Map();
        }

        auto globalSetLayout =
            DescriptorSetLayout::Builder(m_Device)
            .AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .Build();

        Texture vikingTexture("../Assets/Textures/viking_room.png", m_Device);
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = vikingTexture.GetImageLayout();
        imageInfo.imageView = vikingTexture.GetImageView();
        imageInfo.sampler = vikingTexture.GetSampler();
        //imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i = 0; i < globalDescriptorSets.size(); i++)
        {
            auto bufferInfo = uboBuffers[i]->DescriptorInfo();
            DescriptorWriter(*globalSetLayout, *m_GlobalPool)
                .WriteBuffer(0, &bufferInfo)
                .WriteImage(1, &imageInfo)
                .Build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{
            m_Device,
            m_Renderer.GetSwapChainRenderPass(),
            globalSetLayout->GetDescriptorSetLayout()
        };

        PointLightSystem pointLightSystem{
			m_Device,
			m_Renderer.GetSwapChainRenderPass(),
			globalSetLayout->GetDescriptorSetLayout()
		};

        Camera camera{};
        auto cameraObject = GameObject::CreateGameObject();
        cameraObject.transform.position = glm::vec3{ 0.0f, -2.0f, 1.0f };
        cameraObject.transform.rotation = glm::vec3{ 0.4f, 0.0f, 0.0f };
        KeyboardMovementController cameraController{};
        //MouseMovementController mouseController{};

        auto currentTime = std::chrono::high_resolution_clock::now();
        float timer = 0;
        //SimpleRenderSystem LineListRenderSystem{ m_Device, m_Renderer.GetSwapChainRenderPass(), VK_PRIMITIVE_TOPOLOGY_LINE_LIST };
        while (!m_Window.Closed())
        {
            m_Window.Update();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            currentTime = newTime;

            timer += frameTime;

            cameraController.MoveInPlaneXY(m_Window.GetWindow(), frameTime, cameraObject);
            //mouseController.UpdateMouse(m_Window.GetWindow(), frameTime, cameraObject);

            camera.LookAt(
                cameraObject.transform.position,
                cameraObject.transform.position + cameraObject.transform.GetForwardVector(),
                glm::vec3{ 0.0f, 0.0f, 1.0f }
                );

            float aspect = m_Renderer.GetAspectRatio();
            camera.SetPerspectiveProjection(glm::radians(60.f), aspect, .1f, 50.f);

            if (auto commandBuffer = m_Renderer.BeginFrame())
            {
                int frameIndex = m_Renderer.GetFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    m_GameObjects
                };
                // Update
                GlobalUbo ubo{};
                ubo.projection = camera.GetProjectionMatrix();
                ubo.view = camera.GetViewMatrix();
                ubo.inverseView = camera.GetInverseViewMatrix();
                pointLightSystem.Update(frameInfo, ubo, timer);
                uboBuffers[frameIndex]->WriteToBuffer(&ubo);
                uboBuffers[frameIndex]->Flush();
                // Render
                m_Renderer.BeginSwapChainRenderPass(commandBuffer);

                // order matters
                simpleRenderSystem.RenderGameObjects(frameInfo);
                pointLightSystem.Render(frameInfo);

                //LineListRenderSystem.RenderGameObjects(frameInfo, m_LineListGameObjects);
                m_Renderer.EndSwapChainRenderPass(commandBuffer);
                m_Renderer.EndFrame();
            }
        }
        vkDeviceWaitIdle(m_Device.GetDevice());
    }

    std::unique_ptr<Model> createXYZ(Device& device, glm::vec3 offset)
    {
        Model::Builder modelBuilder{};
        modelBuilder.vertices = {

		// x axis (red)
		{{0.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},
		{{1.0f, 0.0f, 0.0f}, {.8f, .1f, .1f}},

		// y axis (green)
		{{0.0f, 0.0f, 0.0f}, {.1f, .8f, .1f}},
		{{0.0f, 1.0f, 0.0f}, {.1f, .8f, .1f}},

		// z axis (blue)
		{{0.0f, 0.0f, 0.0f}, {.1f, .1f, .8f}},
		{{0.0f, 0.0f, 1.0f}, {.1f, .1f, .8f}},

		};

        for (auto& v : modelBuilder.vertices) {
			v.position += offset;
		}

        modelBuilder.indices = {
			0, 1, 2, 3, 4, 5
		};

		return std::make_unique<Model>(device, modelBuilder);
    }

    void Application::LoadGameObjects()
    {
        const std::shared_ptr<Model> vikingRoom =
            Model::CreateModelFromFile(m_Device, "../Assets/Models/viking_room.obj");
        auto gameObject = GameObject::CreateGameObject();
        gameObject.model = vikingRoom;
        gameObject.transform.position = { 2.f, 2.0f, 0.1f };
        gameObject.transform.rotation = glm::vec3{ 0.0f, 0.0f, -135.f };
        m_GameObjects.emplace(gameObject.GetId(), std::move(gameObject));

        const std::shared_ptr<Model> smoothVase =
            Model::CreateModelFromFile(m_Device, "../Assets/Models/smooth_vase.obj");
        auto gameObject2 = GameObject::CreateGameObject();
        gameObject2.model = smoothVase;
        gameObject2.transform.position = { -0.5f, 0.f, 0.0f };
        gameObject2.transform.rotation = glm::vec3{ -90.0f, 0.0f, 0.0f };
        gameObject2.transform.scale = { 2.f, 1.f, 2.f };
        m_GameObjects.emplace(gameObject2.GetId(), std::move(gameObject2));

        const std::shared_ptr<Model> flatVase =
            Model::CreateModelFromFile(m_Device, "../Assets/Models/flat_vase.obj");
        auto gameObject3 = GameObject::CreateGameObject();
        gameObject3.model = flatVase;
        gameObject3.transform.position = { 0.5f, 0.f, 0.0f };
        gameObject3.transform.rotation = glm::vec3{ -90.0f, 0.0f, 0.0f };
        gameObject3.transform.scale = { 2.f, 1.f, 2.f };
        m_GameObjects.emplace(gameObject3.GetId(), std::move(gameObject3));

        const std::shared_ptr<Model> quadmodel = 
            Model::CreateModelFromFile(m_Device, "../assets/models/quad.obj");
        auto quad = GameObject::CreateGameObject();
        quad.model = quadmodel;
        quad.transform.position = { 0.f, 0.f, 0.f };
        quad.transform.rotation = glm::vec3{ -90.0f, 0.0f, 0.0f };
        quad.transform.scale = { 3.f, 1.f, 3.f };
        m_GameObjects.emplace(quad.GetId(), std::move(quad));

        std::vector<glm::vec3> lightColors{
             {1.f, .1f, .1f}, // red
             {1.f, .5f, .1f}, // orange
			 {1.f, 1.f, .1f}, // yellow
             {.1f, 1.f, .1f}, // green
             {.1f, 1.f, 1.f}, // cyan
             {.1f, .1f, 1.f}, // blue
			 {1.f, .1f, 1.f}, // magenta
             {1.f, 1.f, 1.f}, // white
        };

        for (int i = 0; i < lightColors.size(); i++)
        { 
            auto pointLight = GameObject::MakePointLight(0.2);
            pointLight.color = lightColors[i];
            auto rotate = glm::rotate(
                glm::mat4(1.0f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                glm::vec3{ 0.f, 0.f, 1.f }
            );
            pointLight.transform.position = glm::vec3(rotate * glm::vec4(1, 1, 1, 1));
            m_GameObjects.emplace(pointLight.GetId(), std::move(pointLight));
        }
    }

}
