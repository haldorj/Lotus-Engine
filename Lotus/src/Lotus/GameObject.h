#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "Renderer/Model.h"

#include <unordered_map>

#define GLM_FORCE_RADIANS

namespace Lotus
{
	struct TransformComponent
	{
	public:
		glm::vec3 position{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};

		glm::mat4 GetTransform() const
		{
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), position);
			glm::mat4 rotationMatrix = GetRotationMatrix();
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), scale);

			return translationMatrix * rotationMatrix * scaleMatrix;
		}
		
		glm::mat3 GetNormalMatrix() const
		{
			return glm::mat3(glm::transpose(glm::inverse(GetTransform())));
		}

		glm::vec3 GetForwardVector() const
		{
			return glm::vec3(glm::vec4(0.f, 1.f, 0.f, 1.f) * GetRotationMatrix());
		};

		glm::vec3 GetRightVector() const
		{
			return glm::vec3(glm::vec4(1.f, 0.f, 0.f, 1.f) * GetRotationMatrix());
		};

		glm::vec3 GetUpVector() const
		{
			return glm::vec3(glm::vec4(0.f, 0.f, 1.f, 1.f) * GetRotationMatrix());
		};

	private:
		glm::mat4 GetRotationMatrix() const;
	};

	struct PointLightComponent {
		float lightIntensity = 1.0f;

	};

	class GameObject
	{
	public:
		using id_t = unsigned int;
		using Map = std::unordered_map<id_t, GameObject>;

		static GameObject CreateGameObject() {
			static id_t currentId = 0;
			return GameObject{ currentId++ };
		}

		static GameObject MakePointLight(
			float intensity = 10.f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f)
		);

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		id_t GetId() { return id; }

		
		glm::vec3 color{};
		TransformComponent transform{};

		std::shared_ptr<Model> model{};
		std::unique_ptr<PointLightComponent> pointLight = nullptr;

	private:
		GameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}