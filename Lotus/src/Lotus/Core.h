#pragma once

#ifdef LOTUS_PLATFORM_WINDOWS
#ifdef LOTUS_DYNAMIC_LINK
#ifdef LOTUS_BUILD_DLL
#define LOTUS_API __declspec(dllexport)
#else
#define LOTUS_API __declspec(dllimport)
#endif
#else
#define LOTUS_API
#endif
#else
#error Lotus only supports Windows!
#endif

#ifdef LOTUS_DEBUG
#define LOTUS_ENABLE_ASSERTS
#endif

#ifdef LOTUS_ENABLE_ASSERTS
#define LOTUS_ASSERT(x, ...) { if(!(x)) { LOTUS_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define LOTUS_CORE_ASSERT(x, ...) { if(!(x)) { LOTUS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define LOTUS_ASSERT(x, ...)
#define LOTUS_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define LOTUS_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)