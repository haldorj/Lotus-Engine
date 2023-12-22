#pragma once

#include "Core.h"
#include "Window/Window.h"

#include "glm/glm.hpp"

namespace Lotus {

	class LOTUS_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		bool m_Running;
		Window m_Window;
	private:
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication();

}
