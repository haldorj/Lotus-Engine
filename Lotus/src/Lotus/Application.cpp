#include "lotuspch.h"
#include "Application.h"

#include "Lotus/Log.h"

#include "glm/glm.hpp"

namespace Lotus {

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		LOTUS_CORE_INFO("HELLO WORLD");
	}


	Application::~Application()
	{

	}

	void Application::Run()
	{
		while (m_Running)
		{

		}
	}

}