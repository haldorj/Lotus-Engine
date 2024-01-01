#pragma once

#include "Device.h"

#include <vulkan/vulkan.h>


namespace Lotus {

	class Texture
	{
	public:
		Texture() = default;
		Texture(std::string filePath, Device& device);
		~Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;
		Texture(Texture&&) = delete;
		Texture& operator=(Texture&&) = delete;

		void Test(std::string fileath, Device& device);
		void CreateTextureImage(std::string filePath, Device& device);
		void CreateImage(Device& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
						VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		void CreateTextureImageView(Device& device);
		void CreateTextureSampler(Device& device);

		VkImageView GetImageView() const { return m_TextureImageView; }
		VkSampler GetSampler() const { return m_TextureSampler; }
		VkImageLayout GetImageLayout() const { return m_TextureLayout; }

	private:
		int m_Width, m_Height, m_MipLevels;

		Device& m_Device;
		VkImage m_TextureImage;
		VkDeviceMemory m_TextureImageMemory;

		VkImageView m_TextureImageView;
		VkSampler m_TextureSampler;
		VkImageLayout m_TextureLayout;

		uint32_t m_LayerCount = 1;
	};

}
