#pragma once

#include "Window/Window.h"
#include <vector>

namespace Lotus {

	class VulkanDevice
	{

#ifdef NDEBUG // If in release mode
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

	public:
		VulkanDevice();
		~VulkanDevice();

		void CreateInstance();
		void SetupDebugMessenger();

		// Helper functions
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	private:
		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		
		const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	};

}

