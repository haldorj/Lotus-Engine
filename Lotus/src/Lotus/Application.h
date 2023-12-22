#pragma once

#include "Core.h"
#include "Window/Window.h"

#include "glm/glm.hpp"
#include <Renderer/VulkanDevice.h>

namespace Lotus {

	class LOTUS_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		Window m_Window;

		VulkanDevice m_Device;

	private:
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication();

}
