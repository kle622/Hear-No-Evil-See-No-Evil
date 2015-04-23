#include "Camera.h"

Camera::Camera(Handles *handles, glm::vec3 lookat, glm::vec3 eye, glm::vec3 up,
               float fov, float aspect, float _near, float _far)
{
    this->handles = handles;
    this->lookat = lookat;
    this->eye = eye;
    this->up = up;
    this->fov = fov;
    this->aspect = aspect;
    this->_near = _near;
    this->_far = _far;
}

void Camera::setView() {
  glm::mat4 Cam = glm::lookAt(this->eye, this->lookat, this->up);

  if (this->handles->uViewMatrix >= 0)
    glUniformMatrix4fv(this->handles->uViewMatrix, 1, GL_FALSE, glm::value_ptr(Cam));
}

void Camera::setProjection() {
    glm::mat4 Projection = glm::perspective(this->fov, this->aspect, this->_near, this->_far);
    if (this->handles->uProjMatrix >= 0)
        glUniformMatrix4fv(this->handles->uProjMatrix, 1, GL_FALSE, glm::value_ptr(Projection));
}

glm::vec3 Camera::getForward()
{
  return glm::normalize(this->lookat - this->eye);
}
glm::vec3 Camera::getStrafe()
{
  return glm::normalize(glm::cross(this->lookat - this->eye, this->up));
}
glm::vec3 Camera::getUp()
{
  return glm::normalize(this->up);
}

double clamp(double x, double min, double max) {
  return x < min ? min : (x > max ? max : x);
}
