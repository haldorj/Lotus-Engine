#pragma once

#include "Core.h"
#include "Log.h"
#include "Window/Window.h"
#include "Renderer/Device.h"
#include "Renderer/SwapChain.h"
#include "Renderer/Pipeline.h"
#include "Lotus/GameObject.h"

namespace Lotus {

    class Application
    {
    public:
        Application();
        ~Application();

        Application(const Application&) = delete; // delete copy constructor
        void operator=(const Application&) = delete; // delete copy operator

        static constexpr uint32_t WIDTH = 1280;
        static constexpr uint32_t HEIGHT = 720;

        void Run();

    private:
        void LoadGameObjects();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void FreeCommandBuffers();
        void DrawFrame();
        void RecreateSwapChain();
        void RecordCommandBuffer(int imageIndex);
        void RenderGameObjects(VkCommandBuffer commandBuffer);

    private:
        Window m_Window{ "Lotus Engine", WIDTH, HEIGHT };
        Device m_Device{ m_Window };
        std::unique_ptr<SwapChain> m_SwapChain;

        std::unique_ptr<Pipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout;
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::vector<GameObject> m_GameObjects;
    };

    // To be defined in Client
    Application* CreateApplication();


}
