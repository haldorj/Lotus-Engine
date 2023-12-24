#pragma once

#include "glm/ext/matrix_transform.hpp"
#include "Renderer/Model.h"

namespace Lotus
{
	struct TransformComponent
	{
		glm::vec3 translation{};
		glm::vec3 scale{ 1.f, 1.f, 1.f };
		glm::vec3 rotation{};
		
		glm::mat4 GetTransform() const
		{
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.f), translation);
			glm::mat4 rotationMatrix =
				glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1.f, 0.f, 0.f)) *
				glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
				glm::rotate(glm::mat4(1.f), rotation.z, glm::vec3(0.f, 0.f, 1.f));
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.f), scale);

			return translationMatrix * rotationMatrix * scaleMatrix;
		}

		glm::vec3 GetForwardVector() const
		{
			glm::mat4 rotationMatrix =
				glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1.f, 0.f, 0.f)) *
				glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
				glm::rotate(glm::mat4(1.f), rotation.z, glm::vec3(0.f, 0.f, 1.f));
			return glm::vec3(glm::vec4(0.f, 1.f, 0.f, 1.f) * rotationMatrix);
		};

		glm::vec3 GetRightVector() const
		{
			glm::mat4 rotationMatrix =
				glm::rotate(glm::mat4(1.f), rotation.x, glm::vec3(1.f, 0.f, 0.f)) *
				glm::rotate(glm::mat4(1.f), rotation.y, glm::vec3(0.f, 1.f, 0.f)) *
				glm::rotate(glm::mat4(1.f), rotation.z, glm::vec3(0.f, 0.f, 1.f));
			return glm::vec3(glm::vec4(1.f, 0.f, 0.f, 1.f) * rotationMatrix);
		}
	};


	class GameObject
	{
	public:
		using id_t = unsigned int;

		static GameObject CreateGameObject() {
			static id_t currentId = 0;
			return GameObject{ currentId++ };
		}
		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) = default;
		GameObject& operator=(GameObject&&) = default;

		id_t GetId() { return id; }

		std::shared_ptr<Model> model{};
		glm::vec3 color{};
		TransformComponent transform{};

	private:
		GameObject(id_t objId) : id{ objId } {}

		id_t id;
	};
}