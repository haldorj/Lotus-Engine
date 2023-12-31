#include "lotuspch.h"
#include "Texture.h"
#include "Buffer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Lotus {

    Texture::Texture(std::string filePath, Device& device)
		: m_Device(device)
    {
        //Test(filePath, m_Device);
        CreateTextureImage(filePath, m_Device);
        CreateTextureImageView(m_Device);
        CreateTextureSampler(m_Device);
    }

    Texture::~Texture()
    {
        vkDestroySampler(m_Device.GetDevice(), m_TextureSampler, nullptr);
        vkDestroyImageView(m_Device.GetDevice(), m_TextureImageView, nullptr);
        vkDestroyImage(m_Device.GetDevice(), m_TextureImage, nullptr);
        vkFreeMemory(m_Device.GetDevice(), m_TextureImageMemory, nullptr);
    }

    void Texture::Test(std::string filepath, Device& device)
    {
        int channels;
        int m_BytesPerPixel;

        auto data = stbi_load(filepath.c_str(), &m_Width, &m_Height, &m_BytesPerPixel, 4);

        Buffer stagingBuffer{
            device,
            4,
            static_cast<uint32_t>(m_Width * m_Height),
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        };

        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer(data);

        m_TextureFormat = VK_FORMAT_R8G8B8A8_SRGB;

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = m_TextureFormat;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.extent = { static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), 1 };
        imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;


        device.CreateImageWithInfo(imageInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_TextureImage, m_TextureImageMemory);

        device.TransitionImageLayout(m_TextureImage, m_TextureFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1);

        device.CopyBufferToImage(stagingBuffer.GetBuffer(), m_TextureImage, static_cast<uint32_t>(m_Width), static_cast<uint32_t>(m_Height), 1);

        m_TextureLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 4.0f;
        samplerInfo.maxAnisotropy = 4.0;
        samplerInfo.anisotropyEnable = VK_TRUE;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        vkCreateSampler(device.GetDevice(), &samplerInfo, nullptr, &m_TextureSampler);

        VkImageViewCreateInfo imageViewInfo{};
        imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewInfo.format = m_TextureFormat;
        imageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
        imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewInfo.subresourceRange.baseMipLevel = 0;
        imageViewInfo.subresourceRange.baseArrayLayer = 0;
        imageViewInfo.subresourceRange.layerCount = 1;
        imageViewInfo.subresourceRange.levelCount = 1;
        imageViewInfo.image = m_TextureImage;

        vkCreateImageView(device.GetDevice(), &imageViewInfo, nullptr, &m_TextureImageView);

        stbi_image_free(data);
}

    void Texture::CreateTextureImage(std::string filePath, Device& device)
	{
        // Loading an image /////////////////////////////
        int texChannels;
        // Forces the image to be loaded with an alpha channel, even if it doesn't have one
        stbi_uc* pixels = stbi_load(filePath.c_str(), &m_Width, &m_Height, &texChannels, STBI_rgb_alpha);

        if (!pixels) {
            throw std::runtime_error("failed to load texture image!");
        }

        // Creating a staging buffer /////////////////////////////

        Buffer stagingBuffer(
			device,
            4,
			static_cast<uint32_t>(m_Width * m_Height),
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		);

        stagingBuffer.Map();
        stagingBuffer.WriteToBuffer(pixels);

        stbi_image_free(pixels);

        // Creating the image /////////////////////////////

        m_TextureFormat = VK_FORMAT_R8G8B8A8_SRGB;

        CreateImage(
            device,
            m_Width,
            m_Height,
            m_TextureFormat,
            VK_IMAGE_TILING_OPTIMAL, 
            VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, 
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_TextureImage,
            m_TextureImageMemory
        );

        // Copying buffer to image /////////////////////////////

        device.TransitionImageLayout(
            m_TextureImage, 
            m_TextureFormat,
            VK_IMAGE_LAYOUT_UNDEFINED, 
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
            m_LayerCount
        );

        device.CopyBufferToImage(
            stagingBuffer.GetBuffer(), 
            m_TextureImage, 
            static_cast<uint32_t>(m_Width), 
            static_cast<uint32_t>(m_Height), 
            m_LayerCount
        );

        device.TransitionImageLayout(
            m_TextureImage, 
            m_TextureFormat, 
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            m_LayerCount
        );

	}

    void Texture::CreateImage(Device& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
    {
        VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D; // 2D image
		imageInfo.extent.width = width; // width of image extent
		imageInfo.extent.height = height; // height of image extent
		imageInfo.extent.depth = 1; // 1 because 2D image
		imageInfo.mipLevels = 1; // no mip mapping
		imageInfo.arrayLayers = 1; // number of layers in image array
		imageInfo.format = format; // format of image data
		imageInfo.tiling = tiling; // optimal for GPU access
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED; // don't care about initial layout
		imageInfo.usage = usage; // transfer to it and sample from it
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // only used by one queue family
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // no multisampling
		imageInfo.flags = 0; // no flags

        device.CreateImageWithInfo(imageInfo, properties, image, imageMemory);
    }

    void Texture::CreateTextureImageView(Device& device)
    {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = m_TextureImage;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(device.GetDevice(), &viewInfo, nullptr, &m_TextureImageView) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture image view!");
        }
    }

    void Texture::CreateTextureSampler(Device& device)
    {
        VkSamplerCreateInfo samplerInfo{};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_NEAREST; // how to render when image is magnified on screen
		samplerInfo.minFilter = VK_FILTER_NEAREST; // how to render when image is minified on screen
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT; // how to handle texture coordinates outside of [0, 1] range in U direction
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT; // how to handle texture coordinates outside of [0, 1] range in V direction
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT; // how to handle texture coordinates outside of [0, 1] range in W direction
		samplerInfo.anisotropyEnable = VK_TRUE; // enable anisotropy

        VkPhysicalDeviceProperties properties{}; // physical device properties
        vkGetPhysicalDeviceProperties(m_Device.GetPhysicalDevice(), &properties); // get physical device properties

		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy; // anisotropy value
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK; // border beyond texture
		samplerInfo.unnormalizedCoordinates = VK_FALSE; // whether coords should be normalized between 0 and 1
		samplerInfo.compareEnable = VK_FALSE; // whether to compare texels to a value
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS; // type of comparison

		// mipmapping
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR; // how to handle mipmapping
		samplerInfo.minLod = 0.0f; // lower limit of mip level to use
		samplerInfo.maxLod = 0.0f; // upper limit of mip level to use
		samplerInfo.mipLodBias = 0.0f; // bias for mip level

        if (vkCreateSampler(device.GetDevice(), &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture sampler!");
		}
    }

}