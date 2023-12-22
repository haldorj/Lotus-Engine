#pragma once

#ifdef LOTUS_PLATFORM_WINDOWS

extern Lotus::Application* Lotus::CreateApplication();

int main(int argc, char** argv)
{
	Lotus::Log::Init();
	LOTUS_CORE_WARN("Initialized Log!");
	int a = 5;
	int b = 6;
	LOTUS_INFO("Hello! Var = {0} {1}", a, b);

	printf("Lotus Engine\n");

	auto app = Lotus::CreateApplication();
	app->Run();
	delete app;
}

#endif // LOTUS_PLATFORM_WINDOWS