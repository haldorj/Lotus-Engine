#include "lotuspch.h"
#include "Application.h"

#include "Lotus/Log.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanRenderer/VulkanRenderer.h"

namespace Lotus {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;
	GLFWwindow* m_Window;

	VulkanRenderer vulkanRenderer;

	Application::Application()
		: m_Running{true}
	{
		LOTUS_CORE_INFO("HELLO WORLD");

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

		//uint32_t extensionCount = 0;
		//vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		//std::cout << extensionCount << " extensions supported\n";

		if (vulkanRenderer.init(m_Window) == EXIT_FAILURE)
		{
			return;
		}
	}


	Application::~Application()
	{

		glfwDestroyWindow(m_Window);

		glfwTerminate();

		vulkanRenderer.cleanup();
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_Window))
		{
			glfwPollEvents();
		}
	}

}