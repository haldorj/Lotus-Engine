#pragma once
#include "Device.h"
#include "Buffer.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace Lotus {

	class Model
	{
	public:
		struct Vertex
		{
			glm::vec3 position{};
			glm::vec3 color{};
			glm::vec3 normal{};
			glm::vec2 texCoord{};

			static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
			static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

			bool operator==(const Vertex& other) const
			{
				return position == other.position && color == other.color && normal == other.normal && texCoord == other.texCoord;
			}
		};

		struct Builder
		{
			std::vector<Vertex> vertices;
			std::vector<uint32_t> indices;

			void LoadModel(const std::string& filepath);
		};

		Model(Device& device, const Builder& builder);
		~Model();

		Model(const Model&) = delete; // delete copy constructor
		Model operator=(const Model&) = delete; // delete copy operator

		static std::unique_ptr<Model> CreateModelFromFile(Device& device, const std::string& filepath);

		void Bind(VkCommandBuffer commandBuffer) const;
		void Draw(VkCommandBuffer commandBuffer) const;
	private:
		void CreateVertexBuffers(const std::vector<Vertex>& vertices);
		void CreateIndexBuffers(const std::vector<uint32_t>& indices);

	private:
		Device& m_Device;

		std::unique_ptr<Buffer> m_VertexBuffer;
		uint32_t m_VertexCount;

		bool m_HasIndexBuffer;
		std::unique_ptr<Buffer> m_IndexBuffer;
		uint32_t m_IndexCount;
	};

}