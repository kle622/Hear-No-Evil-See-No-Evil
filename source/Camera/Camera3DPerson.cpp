#include "Camera3DPerson.h"
#define _USE_MATH_DEFINES
#include <cmath>

#define PI 3.141592654
//#define DEBUG

// note: calling getEye() form constructor causes crash
Camera3DPerson::Camera3DPerson(WorldGrid *world, GameObject *focus, float zoom,
                               float fov, float aspect, float _near, float _far, DebugDraw *debug) :
                               Camera(focus->position, focus->position + glm::vec3(0.0f, 0.0f, zoom),
                               glm::vec3(0.0f, 1.0f, 0.0f), fov, aspect, _near, _far, debug)
{
  // when both angles are 0, the camera is directly behind the player, horizontal
  this->phi = 0;  // increase phi to look down
  this->theta = 0;  // increase theta to look left
  this->world = world;
  this->upperBound = UPPER_BOUND_DEFAULT;  // bound is stored in radians
  this->lowerBound = LOWER_BOUND_DEFAULT;  // bound is stored in radians
  this->focus = focus;
  this->zoom = zoom;
  this->minZoom = 0.7f;
  this->offset = glm::vec3(0.0f, 0.7f, 0.0f);
}

// note: calling getEye() from constructor causes crash because setZoom() queries the WorldGrid for collision checking
glm::vec3 Camera3DPerson::getEye()
{
  glm::vec3 res = glm::vec3(cos(theta) * cos(phi),
      sin(phi),
      sin(theta) * cos(phi));

  assertNormalized(res);
  res = this->setZoom(res);
  res += this->lookat;
  return res;
}

/* takes a vector from the lookat point to the target camera position and returns an adjusted vector
 *
 * this value will be between the target position and the lookat point
 *
 * near-plane camera collision model from:
 * http://gamasutra.com/blogs/EricUndersander/20131001/201382/Accurate_Collision_Zoom_for_Cameras.php
 *
 * assume outVec is normalized; zoom will be set using internal knowledge of target zoom value
*/
glm::vec3 Camera3DPerson::setZoom(glm::vec3 outVec)
{
  std::vector<glm::vec3> nearCorners;
  // get corners of near plane
  // http://gamedev.stackexchange.com/questions/19774/determine-corners-of-a-specific-plane-in-the-frustum
  float nearHeight = 2 * tan(this->fov * PI / (2 * 180)) * this->_near;
  float nearWidth = nearHeight * this->aspect;
  // place near center at focal point
  // TODO feel like this near plane calculation is off, look at fixing it
  glm::vec3 nearCenter = this->lookat;
  glm::vec3 upRight = nearCenter + (this->getUp() * nearHeight / 2.0f) + (this->getStrafe() * nearWidth / 2.0f);
  glm::vec3 upLeft = nearCenter + (this->getUp() * nearHeight / 2.0f) - (this->getStrafe() * nearWidth / 2.0f);
  glm::vec3 downRight = nearCenter - (this->getUp() * nearHeight / 2.0f) + (this->getStrafe() * nearWidth / 2.0f);
  glm::vec3 downLeft = nearCenter - (this->getUp() * nearHeight / 2.0f) - (this->getStrafe() * nearWidth / 2.0f);
  nearCorners.push_back(upRight);
  nearCorners.push_back(upLeft);
  nearCorners.push_back(downRight);
  nearCorners.push_back(downLeft);
  nearCorners.push_back(nearCenter);  // helps mitigate camera clipping through outside wall corners

#ifdef DEBUG
  this->debug->addLine(upRight, upLeft, glm::vec3(0.0f, 0.0f, 1.0f), false);
  this->debug->addLine(upRight, downRight, glm::vec3(0.0f, 0.0f, 1.0f), false);
  this->debug->addLine(upLeft, downLeft, glm::vec3(0.0f, 0.0f, 1.0f), false);
  this->debug->addLine(downRight, downLeft, glm::vec3(0.0f, 0.0f, 1.0f), false);
#endif

  std::vector<shared_ptr<GameObject>> objects = this->world->getCloseObjects(this->eye, 1);
  //std::vector<shared_ptr<GameObject>> objects = this->world->wallList;
  std::vector<shared_ptr<GameObject>> walls = this->world->wallList;
  objects.insert(objects.begin(), walls.begin(), walls.end());  // whyyyyyyyyyyy

  // raycast from each corner, get minimum fraction
  float minRayDist = numeric_limits<double>::max();
  for (auto corner = nearCorners.begin(); corner != nearCorners.end(); ++corner) {
    glm::vec3 rayStart = *corner;
#ifdef DEBUG
    this->debug->addLine(rayStart, rayStart + this->zoom * outVec, glm::vec3(0.0f, 0.0f, 1.0f), false);
#endif
    float rayHitDist = this->castRayOnObjects(rayStart, outVec, objects);
    minRayDist = fmin(rayHitDist, minRayDist);
    // collide with ground at y = -1
    if (outVec.y < 0.0f) {  // only necessary if camera is below lookat; also makes sure outVec.y != 0
      float groundIntersect = (-1.0f - rayStart.y) / outVec.y;
      minRayDist = fmin(minRayDist, groundIntersect);
    }
  }
  // given shortest distance from near plane corner to potential collision, determine if zoom should change
  // make sure to handle case where no intersections occured (minRayDist should equal double max in this case)
  minRayDist = fmin(minRayDist, this->zoom);

#ifdef DEBUG
  this->debug->addLine(upRight + outVec * minRayDist, upLeft + outVec * minRayDist, glm::vec3(0.0f, 0.0f, 1.0f), false);
  this->debug->addLine(upRight + outVec * minRayDist, downRight + outVec * minRayDist, glm::vec3(0.0f, 0.0f, 1.0f), false);
  this->debug->addLine(upLeft + outVec * minRayDist, downLeft + outVec * minRayDist, glm::vec3(0.0f, 0.0f, 1.0f), false);
  this->debug->addLine(downRight + outVec * minRayDist, downLeft + outVec * minRayDist, glm::vec3(0.0f, 0.0f, 1.0f), false);
#endif

  return outVec * minRayDist;
}

// casts single ray on all potential collision objects, returns shortest distance to collision
float Camera3DPerson::castRayOnObjects(glm::vec3 rayStart, glm::vec3 rayDirection, std::vector<shared_ptr<GameObject>> objects)
{
  float minLength = numeric_limits<double>::max();
  for (auto iter = objects.begin(); iter != objects.end(); ++iter) {
    // don't check collisions against guards and player
    if (NULL == dynamic_pointer_cast<Guard>(*iter) && NULL == dynamic_pointer_cast<Player>(*iter)) {
      // send bounding box to debug output
#ifdef DEBUG
      this->debug->addBox((*iter)->position, (*iter)->dimensions, glm::vec3(1.0f, 0.64f, 0.0f), true, true);
#endif
      // converting object bounding box to OBB
      OBB obb((*iter)->position, (*iter)->dimensions);
#ifdef DEBUG
      this->debug->addOBB(obb, glm::vec3(1.0f, 0.64f, 0.0f), true, true);
#endif
      float result;
      if (rayOBBIntersect(&result, rayStart, rayDirection, obb)) {
        minLength = fmin(minLength, result);
      }
    }
  }

#ifdef DEBUG
  this->debug->addLine(rayStart, rayStart + rayDirection * minLength, glm::vec3(1.0f, 0.0f, 0.0f), true);
#endif
  return minLength;
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

void Camera3DPerson::update()
{
  this->lookat = this->focus->position + this->offset;
  this->eye = getEye();
}
