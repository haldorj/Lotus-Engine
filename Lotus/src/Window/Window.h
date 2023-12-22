#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace Lotus
{
	class Window
	{
	public:
		Window(const std::string& title, int width, int height);
		~Window();

		Window(const Window&) = delete; //delete copy constructor
		Window& operator=(const Window&) = delete; //delete copy assignment operator

		void Init();
		bool Closed() const { return glfwWindowShouldClose(m_Window); }
		bool Resized() const { return m_FrambufferResized; }
		void ResetResizedFlag() { m_FrambufferResized = false; }
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface) const;
		static void Update();
		int GetWidth() const { return m_Width; }
		int GetHeight() const { return m_Height; }
		GLFWwindow* GetWindow() const { return m_Window; }

		VkExtent2D GetExtent() const { return { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height) }; }
	private:
		static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
	private:
		GLFWwindow* m_Window;
		int m_Width, m_Height;
		std::string m_Title;
		bool m_Closed;
		bool m_FrambufferResized;
	};
}


