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
		: m_Running{true}
	{
		LOTUS_CORE_INFO("HELLO WORLD");

		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		m_Window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::cout << extensionCount << " extensions supported\n";

		glm::mat4 matrix;
		glm::vec4 vec;
		auto test = matrix * vec;
	}


	Application::~Application()
	{
		glfwDestroyWindow(m_Window);

		glfwTerminate();
	}

	void Application::Run()
	{
		while (!glfwWindowShouldClose(m_Window))
		{
			glfwPollEvents();
		}
	}

}