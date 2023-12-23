#pragma once

#include "Core.h"
#include "Log.h"
#include "Window/Window.h"
#include "Renderer/Device.h"
#include "Renderer/SwapChain.h"
#include "Renderer/Pipeline.h"
#include "Renderer/Model.h"

namespace Lotus {

    class Application
    {
    public:
        Application();
        ~Application();

        Application(const Application&) = delete; // delete copy constructor
        void operator=(const Application&) = delete; // delete copy operator

        static constexpr uint32_t WIDTH = 800;
        static constexpr uint32_t HEIGHT = 600;

        void Run();

    private:
        void LoadModels();
        void CreatePipelineLayout();
        void CreatePipeline();
        void CreateCommandBuffers();
        void DrawFrame();

    private:
        Window m_Window{ "Lotus Engine", WIDTH, HEIGHT };
        Device m_Device{ m_Window };
        SwapChain m_SwapChain{ m_Device, m_Window.GetExtent() };

        std::unique_ptr<Pipeline> m_Pipeline;
        VkPipelineLayout m_PipelineLayout{};
        std::vector<VkCommandBuffer> m_CommandBuffers;
        std::unique_ptr<Model> m_Model;
    };

    // To be defined in Client
    Application* CreateApplication();


}
