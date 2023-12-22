workspace "Lotus"
    architecture "x64"
    startproject "Sandbox"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Lotus/vendor/GLFW/include"
IncludeDir["ImGui"] = "Lotus/vendor/imgui"
IncludeDir["glm"] = "Lotus/vendor/glm"
IncludeDir["Vulkan"] = "C:/VulkanSDK/1.3.268.0/Include"

include "Lotus/vendor/GLFW"
include "Lotus/vendor/imgui"

project "Lotus"
    location "Lotus"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "lotuspch.h"
    pchsource "Lotus/src/lotuspch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS"
    }

    includedirs
    {
        "%{prj.name}/src",
        "%{prj.name}/vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.Vulkan}" -- Include Vulkan directory
    }

    links 
    { 
        "GLFW",
        "ImGui",
        "opengl32.lib",
        "C:/VulkanSDK/1.3.268.0/Lib/vulkan-1.lib" -- Link against Vulkan library
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "LOTUS_PLATFORM_WINDOWS",
            "LOTUS_BUILD_DLL",
            "GLFW_INCLUDE_NONE"
        }

    filter "configurations:Debug"
        defines "LOTUS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "LOTUS_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "LOTUS_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "Lotus/vendor/spdlog/include",
        "Lotus/src",
        "Lotus/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.Vulkan}",
        "%{IncludeDir.GLFW}"
    }

    links
    {
        "Lotus",
    }

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "LOTUS_PLATFORM_WINDOWS"
        }

    filter "configurations:Debug"
        defines "LOTUS_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "LOTUS_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "LOTUS_DIST"
        runtime "Release"
        optimize "on"