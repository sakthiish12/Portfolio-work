#include "pch.h"
#include "Be_camera.hpp"
#include <Debugger/Debugger.h>

//// std
//#include <cassert>
//#include <limits>

namespace BE {

void BeCamera::setOrthographicProjection(
    float left, float right, float top, float bottom, float near1, float far1) {
  orthoMatrix = glm::mat4{1.0f};
  orthoMatrix[0][0] = 2.f / (right - left);
  orthoMatrix[1][1] = 2.f / (bottom - top);
  orthoMatrix[2][2] = 1.f / (far1 - near1);
  orthoMatrix[3][0] = -(right + left) / (right - left);
  orthoMatrix[3][1] = -(bottom + top) / (bottom - top);
  orthoMatrix[3][2] = -near1 / (far1 - near1);
}

void BeCamera::setPerspectiveProjection(float fovy, float aspect, float near1, float far1) {
  //assert(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f);
  AssertCore(glm::abs(aspect - std::numeric_limits<float>::epsilon()) > 0.0f, "Assertcore Camera");
  const float tanHalfFovy = tan(fovy / 2.f);
  projectionMatrix = glm::mat4{0.0f};
  projectionMatrix[0][0] = 1.f / (aspect * tanHalfFovy);
  projectionMatrix[1][1] = 1.f / (tanHalfFovy);
  projectionMatrix[2][2] = far1 / (far1 - near1);
  projectionMatrix[2][3] = 1.f;
  projectionMatrix[3][2] = -(far1 * near1) / (far1 - near1);
}

void BeCamera::setViewDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up) {
  const glm::vec3 w{glm::normalize(direction)};
  const glm::vec3 u{glm::normalize(glm::cross(w, up))};
  const glm::vec3 v{glm::cross(w, u)};

  viewMatrix = glm::mat4{1.f};
  viewMatrix[0][0] = u.x;
  viewMatrix[1][0] = u.y;
  viewMatrix[2][0] = u.z;
  viewMatrix[0][1] = v.x;
  viewMatrix[1][1] = v.y;
  viewMatrix[2][1] = v.z;
  viewMatrix[0][2] = w.x;
  viewMatrix[1][2] = w.y;
  viewMatrix[2][2] = w.z;
  viewMatrix[3][0] = -glm::dot(u, position);
  viewMatrix[3][1] = -glm::dot(v, position);
  viewMatrix[3][2] = -glm::dot(w, position);

  inverseViewMatrix = glm::mat4{1.f};
  inverseViewMatrix[0][0] = u.x;
  inverseViewMatrix[0][1] = u.y;
  inverseViewMatrix[0][2] = u.z;
  inverseViewMatrix[1][0] = v.x;
  inverseViewMatrix[1][1] = v.y;
  inverseViewMatrix[1][2] = v.z;
  inverseViewMatrix[2][0] = w.x;
  inverseViewMatrix[2][1] = w.y;
  inverseViewMatrix[2][2] = w.z;
  inverseViewMatrix[3][0] = position.x;
  inverseViewMatrix[3][1] = position.y;
  inverseViewMatrix[3][2] = position.z;
}

void BeCamera::setViewTarget(glm::vec3 position, glm::vec3 target, glm::vec3 up) {
  setViewDirection(position, target - position, up);
}

void BeCamera::setViewYXZ(glm::vec3 position, glm::vec3 rotation) {
  const float c3 = glm::cos(rotation.z);
  const float s3 = glm::sin(rotation.z);
  const float c2 = glm::cos(rotation.x);
  const float s2 = glm::sin(rotation.x);
  const float c1 = glm::cos(rotation.y);
  const float s1 = glm::sin(rotation.y);
  const glm::vec3 u{(c1 * c3 + s1 * s2 * s3), (c2 * s3), (c1 * s2 * s3 - c3 * s1)};
  const glm::vec3 v{(c3 * s1 * s2 - c1 * s3), (c2 * c3), (c1 * c3 * s2 + s1 * s3)};
  const glm::vec3 w{(c2 * s1), (-s2), (c1 * c2)};
  viewMatrix = glm::mat4{1.f};
  viewMatrix[0][0] = u.x;
  viewMatrix[1][0] = u.y;
  viewMatrix[2][0] = u.z;
  viewMatrix[0][1] = v.x;
  viewMatrix[1][1] = v.y;
  viewMatrix[2][1] = v.z;
  viewMatrix[0][2] = w.x;
  viewMatrix[1][2] = w.y;
  viewMatrix[2][2] = w.z;
  viewMatrix[3][0] = -glm::dot(u, position);
  viewMatrix[3][1] = -glm::dot(v, position);
  viewMatrix[3][2] = -glm::dot(w, position);

  inverseViewMatrix = glm::mat4{1.f};
  inverseViewMatrix[0][0] = u.x;
  inverseViewMatrix[0][1] = u.y;
  inverseViewMatrix[0][2] = u.z;
  inverseViewMatrix[1][0] = v.x;
  inverseViewMatrix[1][1] = v.y;
  inverseViewMatrix[1][2] = v.z;
  inverseViewMatrix[2][0] = w.x;
  inverseViewMatrix[2][1] = w.y;
  inverseViewMatrix[2][2] = w.z;
  inverseViewMatrix[3][0] = position.x;
  inverseViewMatrix[3][1] = position.y;
  inverseViewMatrix[3][2] = position.z;
}

}  // namespace BE
