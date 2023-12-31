#pragma once


#include "Utils/Utils.h"
#include <vulkan/vulkan.h>
#include "Device.h"
#include "SwapChain.h"

namespace Lotus {

	class Texture
	{
	public:
		Texture() = default;
		Texture(std::string filePath, Device& device);
		~Texture();

		void CreateTextureImage(std::string filePath, Device& device);
		void CreateImage(Device& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
						VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void CreateTextureImageView(Device& device);
		void CreateTextureSampler(Device& device);

	private:
		Device& m_Device;
		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;

		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;
		VkFormat m_TextureFormat;
		VkImageLayout m_TextureLayout;

		uint32_t m_LayerCount = 1;
	};

}
