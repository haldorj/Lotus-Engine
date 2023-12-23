#include "lotuspch.h"
#include "Window.h"

namespace Lotus {

	Window::Window(const std::string& title, int width, int height)
		: m_Title(title), m_Width(width), m_Height(height)
	{
		Init();
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void Window::Init()
	{
		glfwInit();

		// Set GLFW to not create an OpenGL context
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
		glfwSetWindowUserPointer(m_Window, this);
		glfwSetFramebufferSizeCallback(m_Window, FramebufferResizeCallback);
	}

	void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const
	{
		if (glfwCreateWindowSurface(instance, m_Window, nullptr, surface) != VK_SUCCESS)
		{
			LOTUS_CORE_ERROR("Failed to create window surface");
		}
	}

	void Window::Update()
	{
		glfwPollEvents();
	}

	void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		const auto windowInstance = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
		windowInstance->m_FrambufferResized = true;
		windowInstance->m_Width = width;
		windowInstance->m_Height = height;
	}

}