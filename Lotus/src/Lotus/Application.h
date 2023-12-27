#pragma once

#include "Core.h"
#include "Log.h"
#include "Window/Window.h"
#include "Renderer/Device.h"
#include "Lotus/GameObject.h"
#include "Renderer/Renderer.h"

namespace Lotus {

    class Application
    {
    public:
        Application();
        ~Application();

        Application(const Application&) = delete; // delete copy constructor
        Application operator=(const Application&) = delete; // delete copy operator

        static constexpr uint32_t WIDTH = 1280;
        static constexpr uint32_t HEIGHT = 720;

        void Run();

    private:
        void LoadGameObjects();

    private:
        Window m_Window{ "Lotus Engine", WIDTH, HEIGHT };
        Device m_Device{ m_Window };
        Renderer m_Renderer{ m_Window, m_Device };

        std::vector<GameObject> m_GameObjects;
        std::vector<GameObject> m_LineListGameObjects;
    };

    // To be defined in Client
    Application* CreateApplication();


}
