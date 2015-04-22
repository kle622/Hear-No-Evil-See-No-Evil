#include "Camera3DPerson.h"

Camera3DPerson:Camera3DPerson(Handles *handles, GameObject *focus, float zoom, float fov, float aspect, float _near, float _far)
{
  // when both angles are 0, the camera is directly behind the player, horizontal
  this->pitch = 0;  // increase pitch to look down
  this->focus = focus;
  Camera(handles,
         focus->position,
         getEye(),
         glm::vec3(0.0f, 1.0f, 0.0f),
         fov, aspect, _near, _far);
}

glm::vec3 Camera3DPerson:getEye()
{
  glm::vec3 res = this->focus->direction;
  res.y = 0;
  res = glm::normalize(res);
  float theta = acos(glm::dot(res, glm::vec3(1.0f, 0.0f, 0.0f)));   // theta = 0 points along positive x
  if (res.z < 0) {  // theta increases towards positive z
    theta *= -1.0f;
  }
  res = glm::vec3(cos(theta) * cos(pitch),
                  sin(pitch),
                  sin(theta) * cos(pitch));
  res *= this->zoom;
  res += this->focus->position;
  return res;
}

void Camera3DPerson::moveVert(float step)
{
  this->pitch -= step;
  this->eye = getEye();
}

void Camera3DPerson::checkCollide(vector<shared_ptr<GameObject>> objects)
{
  // TODO implement
}
