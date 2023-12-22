#include "lotuspch.h"
#include "Application.h"

#include "Lotus/Log.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Lotus {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;
	GLFWwindow* m_Window;

	Application::Application()
		: m_Running{true}, m_Window{"Lotus Engine", 1280, 720}
	{
		LOTUS_CORE_ASSERT(!s_Instance, "Application already exists!");
		s_Instance = this;

	}


	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window.Update();
		}
	}

}