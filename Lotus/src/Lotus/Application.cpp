#include "lotuspch.h"
#include "Application.h"
#include "Renderer/SimpleRenderSystem.h"

#include "Lotus/Log.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

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
        SimpleRenderSystem simpleRenderSystem{ m_Device, m_Renderer.GetSwapChainRenderPass() };
        while (!m_Window.Closed())
        {
            m_Window.Update();
            if (auto commandBuffer = m_Renderer.BeginFrame())
            {
                m_Renderer.BeginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.RenderGameObjects(commandBuffer, m_GameObjects);
                m_Renderer.EndSwapChainRenderPass(commandBuffer);
                m_Renderer.EndFrame();
            }
        }
    }

    void Application::LoadGameObjects()
    {
        std::vector<Model::Vertex> vertices{
          {{0.0f, -0.5f}, {0.8f, 0.2f, 0.2f}},
          {{0.5f, 0.5f}, {0.2f, 0.8f, 0.2f}},
          {{-0.5f, 0.5f}, {0.2f, 0.2f, 0.8f}}
        };

        auto model = std::make_shared<Model>(m_Device, vertices);

        auto triangle = GameObject::CreateGameObject();
        triangle.model = model;
        triangle.color = { .1f, .8f, .1f };
        triangle.transform2d.translation.x = .2f;
        triangle.transform2d.scale = { 2.f, .5f };
        triangle.transform2d.rotation = .25f * glm::two_pi<float>();

        m_GameObjects.push_back(std::move(triangle));
    }

}
