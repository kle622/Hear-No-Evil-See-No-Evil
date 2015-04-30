#include "Camera3DPerson.h"

// note: calling getEye() form constructor causes crash
Camera3DPerson::Camera3DPerson(Handles *handles, WorldGrid *world, GameObject *focus, float zoom, float fov, float aspect, float _near, float _far) : Camera(handles, focus->position, focus->position + glm::vec3(0.0f, 0.0f, zoom), glm::vec3(0.0f, 1.0f, 0.0f), fov, aspect, _near, _far)
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

// note: calling getEye() form constructor causes crash
glm::vec3 Camera3DPerson::getEye()
{
  glm::vec3 res = glm::vec3(cos(theta) * cos(phi),
      sin(phi),
      sin(theta) * cos(phi));

  res = adjustZoom(res);
  res += this->lookat;
  return res;
}

// takes a target camera position and returns an adjusted value
// this value will be between the original position and the focus of the camera
glm::vec3 Camera3DPerson::adjustZoom(glm::vec3 outDir)
{
  std::vector<glm::vec3> nearCorners;
  vector<shared_ptr<GameObject>> collidors = this->world->getCloseObjects(
      outDir, 2);
  // get corners of near plane
  float nearHeight = 2 * tan(this->fov / 2) * this->_near;
  float nearWidth = nearHeight * this->aspect;
  glm::vec3 nearCenter = outDir + glm::normalize(this->lookat - outDir) * this->_near;
  nearCorners.push_back(nearCenter + (this->getUp() * nearHeight / 2.0f) + (this->getStrafe() * nearWidth / 2.0f));
  nearCorners.push_back(nearCenter + (this->getUp() * nearHeight / 2.0f) - (this->getStrafe() * nearWidth / 2.0f));
  nearCorners.push_back(nearCenter - (this->getUp() * nearHeight / 2.0f) + (this->getStrafe() * nearWidth / 2.0f));
  nearCorners.push_back(nearCenter - (this->getUp() * nearHeight / 2.0f) - (this->getStrafe() * nearWidth / 2.0f));

  // raycast from each corner, get minimum fraction
  float minRayFract = 1.0f;
  std::vector<glm::vec3>::iterator corner;
  for (corner = nearCorners.begin(); corner != nearCorners.end(); ++corner) {
    glm::vec3 rayStart = *corner;
    glm::vec3 rayEnd = rayStart + outDir;
    float rayHitFract = this->castRay(rayStart, rayEnd);
    minRayFract = rayHitFract < minRayFract ? rayHitFract : minRayFract;
  }

  return outDir * minRayFract;
}

float Camera3DPerson::castRay(glm::vec3 rayStart, glm::vec3 rayEnd)
{
  return 1.0f;
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
  this->lookat = this->focus->position;
  this->eye = getEye();
  Camera::setView();
}
void Camera3DPerson::setProjection()
{
  Camera::setProjection();
}
