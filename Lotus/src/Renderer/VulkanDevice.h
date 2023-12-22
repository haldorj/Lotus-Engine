#pragma once

#include "Window/Window.h"
#include <vector>
#include <optional>

namespace Lotus {

	struct QueueFamilyIndices {
		std::optional<uint32_t> graphicsFamily;

		bool IsComplete() {
			return graphicsFamily.has_value();
		}
	};

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
		void PickPhysicalDevice();
		void CreateLogicalDevice();

		// Helper functions
		bool CheckValidationLayerSupport();
		std::vector<const char*> GetRequiredExtensions();
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
		bool IsDeviceSuitable(VkPhysicalDevice device);
		QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		VkPhysicalDeviceProperties properties;

	private:
		VkInstance m_Instance;
		VkDebugUtilsMessengerEXT m_DebugMessenger;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_Device; // Logical device
		VkQueue m_GraphicsQueue;
		
		const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };

	};

}

