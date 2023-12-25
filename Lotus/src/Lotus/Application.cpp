#include "lotuspch.h"
#include "Application.h"
#include "Renderer/SimpleRenderSystem.h"
#include "Camera/Camera.h"
#include "Input/KeyboardMovementController.h"

#include "Lotus/Log.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <Input/MouseMovementController.h>

namespace Lotus {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)


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
        Camera camera{};
        auto cameraObject = GameObject::CreateGameObject();
        cameraObject.transform.position = glm::vec3{ 0.0f, -2.0f, 1.0f };
        cameraObject.transform.rotation = glm::vec3{ 0.4f, 0.0f, 0.0f };
        KeyboardMovementController cameraController{};
        //MouseMovementController mouseController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        SimpleRenderSystem simpleRenderSystem{ m_Device, m_Renderer.GetSwapChainRenderPass() };
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
                m_Renderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects, camera);
                m_Renderer.EndSwapChainRenderPass(commandBuffer);
                m_Renderer.EndFrame();
            }
        }
    }

    std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset)
    {
        std::vector<Model::Vertex> vertices{

            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},

        };
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<Model>(device, vertices);
    }

    std::unique_ptr<Model> createXYZ(Device& device, glm::vec3 offset)
    {
        std::vector<Model::Vertex> vertices{

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
        for (auto& v : vertices) {
            v.position += offset;
        }
        return std::make_unique<Model>(device, vertices);
    }

    void Application::LoadGameObjects()
    {
        std::shared_ptr<Model> model = createCubeModel(m_Device, glm::vec3{ 0.0f, 0.0f, 0.0f });
         auto cube = GameObject::CreateGameObject();
         cube.model = model;
         cube.transform.position = { 0.0f, 0.0f, 0.0f };
         cube.transform.scale = { .5f, .5f, .5f };
         m_GameObjects.push_back(std::move(cube));

        //std::shared_ptr<Model> XYZmodel = createXYZ(m_Device, glm::vec3{ 0.0f, 0.0f, 0.0f });
        //auto xyz = GameObject::CreateGameObject();
        //xyz.model = XYZmodel;
        //xyz.transform.position = { 0.0f, 0.0f, 0.0f };
        //xyz.transform.scale = { 1.0f, 1.0f, 1.0f };
        //m_GameObjects.push_back(std::move(xyz));
    }

}
