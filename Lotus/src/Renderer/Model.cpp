#include "lotuspch.h"
#include "Model.h"

#include <cassert>
#include <cstring>

#include "Utils/Utils.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace std
{
    template<>
    struct hash<Lotus::Model::Vertex>
    {
        size_t operator()(const Lotus::Model::Vertex& vertex) const
        {
            size_t seed = 0;
            Lotus::hash_combine(seed, vertex.position.x, vertex.position.y, vertex.position.z,
                vertex.color.r, vertex.color.g, vertex.color.b,
                vertex.normal.x, vertex.normal.y, vertex.normal.z,
                vertex.texCoord.x, vertex.texCoord.y);
            return seed;
        }
    };
}


namespace Lotus {

    Model::Model(Device& device, const Builder& builder)
        : m_Device{ device }
    {
        CreateVertexBuffers(builder.vertices);
        CreateIndexBuffers(builder.indices);
    }

    Model::~Model()
    {
        vkDestroyBuffer(m_Device.GetDevice(), m_VertexBuffer, nullptr);
        vkFreeMemory(m_Device.GetDevice(), m_VertexBufferMemory, nullptr);
        if (m_HasIndexBuffer)
        {
            vkDestroyBuffer(m_Device.GetDevice(), m_IndexBuffer, nullptr);
            vkFreeMemory(m_Device.GetDevice(), m_IndexBufferMemory, nullptr);
        }
    }

    std::unique_ptr<Model> Model::CreateModelFromFile(Device& device, const std::string& filepath)
    {
        Builder builder{};
        builder.LoadModel(filepath);
        std::cout << "Model loaded from file: " << filepath << "\n"
            << "Model has " << builder.vertices.size() << " vertices" << std::endl;

        return std::make_unique<Model>(device, builder);
    }

    void Model::Bind(VkCommandBuffer commandBuffer) const
    {
        VkBuffer buffers[] = { m_VertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (m_HasIndexBuffer)
            vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    void Model::Draw(VkCommandBuffer commandBuffer) const
    {
        if (m_HasIndexBuffer)
            vkCmdDrawIndexed(commandBuffer, m_IndexCount, 1, 0, 0, 0);
        else
            vkCmdDraw(commandBuffer, m_VertexCount, 1, 0, 0);
    }

    void Model::CreateVertexBuffers(const std::vector<Vertex>& vertices)
    {
        m_VertexCount = static_cast<uint32_t>(vertices.size());
        assert(m_VertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * m_VertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        m_Device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

        m_Device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_VertexBuffer,
            m_VertexBufferMemory);

        m_Device.CopyBuffer(stagingBuffer, m_VertexBuffer, bufferSize);

        vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
    }

    void Model::CreateIndexBuffers(const std::vector<uint32_t>& indices)
    {
        m_IndexCount = static_cast<uint32_t>(indices.size());
        m_HasIndexBuffer = m_IndexCount > 0;
        if (!m_HasIndexBuffer) return;
        VkDeviceSize bufferSize = sizeof(indices[0]) * m_IndexCount;
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        m_Device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(m_Device.GetDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m_Device.GetDevice(), stagingBufferMemory);

        m_Device.CreateBuffer(
            bufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            m_IndexBuffer,
            m_IndexBufferMemory);

        m_Device.CopyBuffer(stagingBuffer, m_IndexBuffer, bufferSize);

        vkDestroyBuffer(m_Device.GetDevice(), stagingBuffer, nullptr);
        vkFreeMemory(m_Device.GetDevice(), stagingBufferMemory, nullptr);
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

        attributeDescriptions.push_back({ 0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, position) });
        attributeDescriptions.push_back({ 1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color) });
        attributeDescriptions.push_back({ 2, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal) });
        attributeDescriptions.push_back({ 3, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord) });

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    }

    void Model::Builder::LoadModel(const std::string& filepath)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials; // we won't use materials for now
        std::string warn, error;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &error, filepath.c_str()))
        {
            throw std::runtime_error(warn + error);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Vertex vertex{};
                if (index.vertex_index >= 0)
                {
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };
                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
                }
                if (index.normal_index >= 0)
                {
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }
                if (index.texcoord_index >= 0)
                {
                    vertex.texCoord = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1]
                    };
                }

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }
        }
    }

}