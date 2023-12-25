#pragma once
#include "Device.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lotus {

	class Model
	{
	public:
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 color;

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
		};

		struct Builder
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;
		};

		Model(Device& device, const Builder& builder);
		~Model();

		Model(const Model&) = delete; // delete copy constructor
		Model operator=(const Model&) = delete; // delete copy operator

		void Bind(VkCommandBuffer commandBuffer) const;
		void Draw(VkCommandBuffer commandBuffer) const;
	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

	private:
		Device& m_Device;

		VkBuffer m_VertexBuffer;
		VkDeviceMemory m_VertexBufferMemory;
		uint32_t m_VertexCount;

		bool m_HasIndexBuffer;
		VkBuffer m_IndexBuffer;
		VkDeviceMemory m_IndexBufferMemory;
		uint32_t m_IndexCount;
	};

}