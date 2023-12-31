#include "lotuspch.h"
#include "GameObject.h"

namespace Lotus {

    GameObject GameObject::MakePointLight(float intensity, float radius, glm::vec3 color)
    {
        GameObject gameObject = CreateGameObject();
        gameObject.color = color;
        gameObject.transform.scale.x = radius;
        gameObject.pointLight = std::make_unique<PointLightComponent>();
        gameObject.pointLight->lightIntensity = intensity;
        return gameObject;
    }

    glm::mat4 TransformComponent::GetRotationMatrix() const
    {
        glm::mat4 rotationMatrix =
            glm::rotate(glm::mat4(1.f), glm::radians(rotation.x), glm::vec3(1.f, 0.f, 0.f)) *
            glm::rotate(glm::mat4(1.f), glm::radians(rotation.y), glm::vec3(0.f, 1.f, 0.f)) *
            glm::rotate(glm::mat4(1.f), glm::radians(rotation.z), glm::vec3(0.f, 0.f, 1.f));
        return rotationMatrix;
    }

}

