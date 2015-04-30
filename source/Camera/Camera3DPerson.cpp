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

  res *= this->zoom;
  res = adjustZoom(res);
  res += this->lookat;
  return res;
}

/* takes a vector from the lookat point to the target camera position and returns an adjusted vector
 *
 * this value will be between the target position and the lookat point
 *
 * near-plane camera collision model from:
 * http://gamasutra.com/blogs/EricUndersander/20131001/201382/Accurate_Collision_Zoom_for_Cameras.php
*/
glm::vec3 Camera3DPerson::adjustZoom(glm::vec3 outDir)
{
  std::vector<glm::vec3> nearCorners;
  // get corners of near plane
  // http://gamedev.stackexchange.com/questions/19774/determine-corners-of-a-specific-plane-in-the-frustum
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
    minRayFract = MIN(rayHitFract, minRayFract);
  }

  return outDir * minRayFract;
}

float Camera3DPerson::castRay(glm::vec3 rayStart, glm::vec3 rayEnd)
{
  float minFract = 1.0f;
  float rayLength = glm::distance(rayEnd, rayStart);
  glm::vec3 rayDirection = glm::normalize(rayEnd - rayStart);
  std::vector<shared_ptr<GameObject>> objects = this->world->getCloseObjects(this->lookat, 1);
  std::vector<shared_ptr<GameObject>>::iterator iter;
  for (iter = objects.begin(); iter != objects.end(); ++iter) {
    // only check collisions against non-player objects
    if (NULL != dynamic_pointer_cast<Wall>(*iter)) {
      // converting object bounding box to OBB
      OrientedBoundingBox obb;
      obb.center = (*iter)->position;
      obb.axes[0] = glm::vec3(1.0f, 0.0f, 0.0f);
      obb.axes[1] = glm::vec3(0.0f, 1.0f, 0.0f);
      obb.axes[2] = glm::vec3(0.0f, 0.0f, 1.0f);
      obb.halfLengths[0] = (*iter)->dimensions.x;
      obb.halfLengths[1] = (*iter)->dimensions.y;
      obb.halfLengths[2] = (*iter)->dimensions.z;
      float result;
      if (rayOBBIntersect(&result, rayStart, rayDirection, obb)) {
        float rayFract = result / rayLength;
        minFract = minFract < rayFract ? minFract : rayFract;
      }
    }
  }

  return minFract;
}

// uses Kay and Kajiya's slab method for ray/box intersection, found in text p.472
// assume value pointed to by dist is unusable if this method returns false
bool Camera3DPerson::rayOBBIntersect(float *dist, glm::vec3 rayOrigin, glm::vec3 rayDirection, OrientedBoundingBox obb)
{
  *dist = 0;
  float tmin = numeric_limits<double>::min();
  float tmax = numeric_limits<double>::max();
  glm::vec3 p = obb.center - rayOrigin;
  for (int i = 0; i < 3; ++i) {
    float e = glm::dot(obb.axes[i], p);
    float f = glm::dot(obb.axes[i], rayDirection);
    if (f > EPSILON || f < -1.0f * EPSILON) {
      float t1 = (e + obb.halfLengths[i]) / f;
      float t2 = (e - obb.halfLengths[i]) / f;
      if (t1 > t2) {
        // swap t1 and t2
        float temp = t1;
        t1 = t2;
        t2 = temp;
      }
      tmin = MAX(tmin, t1);
      tmax = MIN(tmax, t2);
      if (tmin > tmax || tmax < 0) {
        return false;
      }
    }
    else if (-1.0f * e - obb.halfLengths[i] > 0 || obb.halfLengths[i] - e < 0)
    {
      return false;
    }
  }
  if (tmin > 0) {
    *dist = tmin;
  }
  else {
    *dist = tmax;
  }
  return true;
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
