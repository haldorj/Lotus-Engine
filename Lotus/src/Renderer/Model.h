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
            glm::vec2 position;
            glm::vec3 color;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        Model(Device& device, const std::vector<Vertex>& vertices);
        ~Model();

        Model(const Model&) = delete; // delete copy constructor
        void operator=(const Model&) = delete; // delete copy operator

        void Bind(VkCommandBuffer commandBuffer);
        void Draw(VkCommandBuffer commandBuffer);
    private:
        void CreateVertexBuffers(const std::vector<Vertex>& vertices);

    private:
        Device& m_Device;
        VkBuffer m_VertexBuffer;
        VkDeviceMemory m_VertexBufferMemory;
        uint32_t m_VertexCount;
    };

}