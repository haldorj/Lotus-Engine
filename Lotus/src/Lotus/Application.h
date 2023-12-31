#pragma once

#include "Core.h"
#include "Log.h"
#include "Window/Window.h"
#include "Renderer/Device.h"
#include "GameObject/GameObject.h"
#include "Renderer/Renderer.h"
#include "Renderer/Descriptors.h"
#include "Renderer/Texture.h"

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

        std::unique_ptr<DescriptorPool> m_GlobalPool{};
        GameObject::Map m_GameObjects;
        //std::vector<GameObject> m_LineListGameObjects;
    };

    // To be defined in Client
    Application* CreateApplication();


}
