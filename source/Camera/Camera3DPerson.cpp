#include "Camera3DPerson.h"

Camera3DPerson::Camera3DPerson(Handles *handles, WorldGrid *world, GameObject *focus, float zoom, float fov, float aspect, float _near, float _far) : Camera(handles, focus->position, getEye(), glm::vec3(0.0f, 1.0f, 0.0f), fov, aspect, _near, _far)
{
  // when both angles are 0, the camera is directly behind the player, horizontal
  this->phi = 0;  // increase phi to look down
  this->theta = 0;  // increase theta to look left
  this->world = world;
  this->upperBound = UPPER_BOUND_DEFAULT;  // bound is stored in radians
  this->lowerBound = LOWER_BOUND_DEFAULT;  // bound is stored in radians
  this->focus = focus;
  this->zoom = zoom;
}

glm::vec3 Camera3DPerson::getEye()
{
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
  this->phi += step;
  float upperBoundRad = this->upperBound * PI / 180;
  float lowerBoundRad = this->lowerBound * PI / 180;
  if (this->phi > upperBoundRad) {
    this->phi = upperBoundRad;
  }
  if (this->phi < lowerBoundRad) {
    this->phi = lowerBoundRad;
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
