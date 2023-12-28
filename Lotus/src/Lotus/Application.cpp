#include "lotuspch.h"
#include "Application.h"
#include "Renderer/SimpleRenderSystem.h"
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

    struct GlobalUbo
    {
        glm::mat4 projectionView{ 1.f };
        glm::vec3 lightDirection = glm::normalize(glm::vec3{ 1.f, -3.f, -1.f });
    };


    Application::Application()
    {
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

        Camera camera{};
        auto cameraObject = GameObject::CreateGameObject();
        cameraObject.transform.position = glm::vec3{ 0.0f, -2.0f, 1.0f };
        cameraObject.transform.rotation = glm::vec3{ 0.4f, 0.0f, 0.0f };
        KeyboardMovementController cameraController{};
        //MouseMovementController mouseController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        SimpleRenderSystem simpleRenderSystem{ m_Device, m_Renderer.GetSwapChainRenderPass() };
        SimpleRenderSystem LineListRenderSystem{ m_Device, m_Renderer.GetSwapChainRenderPass(), VK_PRIMITIVE_TOPOLOGY_LINE_LIST };
        while (!m_Window.Closed())
        {
            m_Window.Update();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float>(newTime - currentTime).count();
            currentTime = newTime;
            
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
                FrameInfo frameInfo{ frameIndex, frameTime, commandBuffer, camera };
                // Update
                GlobalUbo ubo{};
                ubo.projectionView = camera.GetProjectionMatrix() * camera.GetViewMatrix();
                uboBuffers[frameIndex]->WriteToBuffer(&ubo);
                uboBuffers[frameIndex]->Flush();
                // Render
                m_Renderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(frameInfo, m_GameObjects);
                LineListRenderSystem.RenderGameObjects(frameInfo, m_LineListGameObjects);
                m_Renderer.EndSwapChainRenderPass(commandBuffer);
                m_Renderer.EndFrame();
            }
        }
    }

    std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset)
    {
        Model::Builder modelBuilder{};
        modelBuilder.vertices = {

            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top face (orange)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            // back face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // top face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // bottom face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},

        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }

        modelBuilder.indices = {
            0, 1, 2, 0, 3, 1,
            4, 5, 6, 4, 7, 5, 
            8, 9, 10, 8, 11, 9,
            12, 13, 14, 12, 15, 13, 
            16, 17, 18, 16, 19, 17, 
            20, 21, 22, 20, 23, 21
        };

        return std::make_unique<Model>(device, modelBuilder);
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
        std::shared_ptr<Model> model = createCubeModel(m_Device, glm::vec3{ 0.0f, 0.0f, 0.0f });
        auto cube = GameObject::CreateGameObject();
        cube.model = model;
        cube.transform.position = { 1.f, 1.f, 0.25f };
        cube.transform.scale = { .5f, .5f, .5f };
        m_GameObjects.push_back(std::move(cube));
         
        std::shared_ptr<Model> XYZmodel = createXYZ(m_Device, glm::vec3{ 0.0f, 0.0f, 0.0f });
        auto xyz = GameObject::CreateGameObject();
        xyz.model = XYZmodel;
        xyz.transform.position = { 0.0f, 0.0f, 0.0f };
        xyz.transform.scale = { 1.0f, 1.0f, 1.0f };
        m_LineListGameObjects.push_back(std::move(xyz));

        const std::shared_ptr<Model> vikingRoom =
            Model::CreateModelFromFile(m_Device, "../Assets/Models/viking_room.obj");

        auto gameObject = GameObject::CreateGameObject();
        gameObject.model = vikingRoom;
        gameObject.transform.position = { 2.5f, 2.5f, 0.0f };
        
        gameObject.transform.rotation = glm::vec3{ 0.0f, 0.0f, -90.f };
        //gameObject.transform.scale = { 2.5f, 2.5f, 2.5f };
        m_GameObjects.push_back(std::move(gameObject));
    }

}
