#include "Camera3DPerson.h"

Camera3DPerson:Camera3DPerson(Handles *handles, glm::vec3 *focus, float zoom, float fov, float aspect, float _near, float _far)
{
  // when both angles are 0, the camera is directly behind the player, horizontal
  this->phi = 0;  // increase phi to look down
  this->theta = 0;  // increase theta to look left
  this->focus = focus;
  Camera(handles,
         *focus,
         getEye(),
         glm::vec3(0.0f, 1.0f, 0.0f),
         fov, aspect, _near, _far);
}

Camera3DPerson:getEye()
{
  glm::vec3 res = glm::vec3(sin(this->theta),
                            sin(this->phi),
                            -1.0f * cos(this->phi) * cos(this->theta));
  res *= this->zoom;
  res += *(this->focus);
  return res;
}

void moveHoriz(float step)
{
  this->theta -= step;
  this->eye = getEye();
}

void moveVert(float step)
{
  this->phi -= step;
  this->eye = getEye();
}

void checkCollide(vector<shared_ptr<GameObject>> objects)
{
  // TODO implement
}
