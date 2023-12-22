#include <Lotus.h>

#include "imgui/imgui.h"

#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/ext/scalar_constants.hpp> // glm::pi


class Sandbox : public Lotus::Application
{
public:
	Sandbox()
	{

	}
	~Sandbox()
	{

	}
};

Lotus::Application* Lotus::CreateApplication()
{
	return new Sandbox();
}