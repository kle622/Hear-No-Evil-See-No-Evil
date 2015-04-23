#include "Camera3DPerson.h"

Camera3DPerson::Camera3DPerson(Handles *handles, GameObject *focus, float zoom, float fov, float aspect, float _near, float _far) : Camera(handles, focus->position, getEye(), glm::vec3(0.0f, 1.0f, 0.0f), fov, aspect, _near, _far)
{
  // when both angles are 0, the camera is directly behind the player, horizontal
  this->phi = 0;  // increase phi to look down
  this->theta = 0;  // increase theta to look left
  this->boundHigh = 85 * M_PI / 180;  // bound is stored in radians
  this->boundLow = -10 * M_PI / 180;  // bound is stored in radians
  this->focus = focus;
  this->zoom = zoom;
  /*Camera(handles,
         focus->position,
         getEye(),
         glm::vec3(0.0f, 1.0f, 0.0f),
         fov, aspect, _near, _far);*/
}

glm::vec3 Camera3DPerson::getEye()
{
  /*glm::vec3 res = this->focus->direction;
  res.y = 0;
  res = glm::normalize(res);
  float theta = acos(glm::dot(res, glm::vec3(1.0f, 0.0f, 0.0f)));   // theta = 0 points along positive x
  if (res.z < 0) {  // theta increases towards positive z
    theta *= -1.0f;
  }
  res = glm::vec3(cos(theta) * cos(phi),
                  sin(phi),
                  sin(theta) * cos(phi));
  res *= this->zoom;
  res += this->focus->position;
  return res;*/
  glm::vec3 res = glm::vec3(cos(theta) * cos(phi),
                            sin(phi),
                            sin(theta) * cos(phi));
  res *= this->zoom;
  res += this->lookat;
  return res;
}

// positive step looks up
void Camera3DPerson::moveVert(float step)
{
  this->phi -= step;
  if (this->phi > this->boundHigh) {
    this->phi = this->boundHigh;
  }
  if (this->phi < this->boundLow) {
    this->phi = this->boundLow;
  }
}

// positive step looks right
void Camera3DPerson::moveHoriz(float step)
{
  this->theta -= step;
}

//Object Methods
void Camera3DPerson::setView()
{
  this->eye = getEye();
  this->lookat = this->focus->position;
  Camera::setView();
}
void Camera3DPerson::setProjection()
{
  Camera::setProjection();
}

void Camera3DPerson::checkCollide(vector<shared_ptr<GameObject>> objects)
{
  // TODO implement
}
