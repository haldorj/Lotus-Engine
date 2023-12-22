#pragma once

#include "glm/glm.hpp"

namespace Lotus {

	class Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		bool m_Running;

	private:
		static Application* s_Instance;
	};

	// To be defined in Client
	Application* CreateApplication();

}
