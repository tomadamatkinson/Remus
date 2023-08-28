#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

namespace remus
{
class Transform
{
  public:
	Transform()  = default;
	~Transform() = default;

	glm::vec3 translation = glm::vec3(0.0f);
	glm::quat rotation    = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec3 scale       = glm::vec3(1.0f);

	glm::mat4 get_matrix() const
	{
		glm::mat4 matrix(1.0f);
		matrix = glm::translate(matrix, translation);
		matrix *= glm::mat4_cast(rotation);
		matrix = glm::scale(matrix, scale);
		return matrix;
	}
};

struct WorldMatrix
{
	glm::mat4 matrix;
};
}        // namespace remus
