#include "Camera.h"
#define _USE_MATH_DEFINES

#define DEBUG

Camera::Camera(Handles *handles, glm::vec3 lookat, glm::vec3 eye, glm::vec3 up,
    float fov, float aspect, float _near, float _far)
{
  this->handles = handles;
  this->lookat = lookat;
  this->eye = eye;
  this->up = up;
  this->fov = fov;
  this->aspect = aspect;
  this->_near= _near;
  this->_far = _far;
}

glm::mat4 Camera::getView()
{
  return glm::lookAt(this->eye, this->lookat, this->up);
}

glm::mat4 Camera::getProjection()
{
  return glm::perspective(this->fov, this->aspect, this->_near, this->_far);
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

std::vector<std::shared_ptr<GameObject>> Camera::getUnculled(WorldGrid *worldgrid)
{
  // ----- View Frustum Culling -----
  std::vector<glm::vec4> *planes = getViewFrustum(getProjection() * getView());
  
#ifdef DEBUG
  this->debug->addBox(planes->at(0), planes->at(1), planes->at(2), planes->at(3), planes->at(4), planes->at(5), glm::vec3(0.99f, 0.85f, 0.55f), false, false);
#endif

  std::vector<std::shared_ptr<GameObject>> allObjects = worldgrid->list;
  std::vector<std::shared_ptr<GameObject>> unculled;
  std::vector<std::shared_ptr<GameObject>> walls = worldgrid->wallList;
  allObjects.insert(allObjects.begin(), walls.begin(), walls.end());  // whyyyyyyyyyyy
  for (auto objIter = allObjects.begin(); objIter != allObjects.end(); ++objIter) {
    OBB *obb = new OBB((*objIter)->position, (*objIter)->dimensions);
    bool pass = true;
    for (auto planeIter = planes->begin(); pass && planeIter != planes->end(); ++planeIter) {
      pass = obbOutsidePlane(*obb, *planeIter);
    }
    if (pass) {
      unculled.push_back(*objIter);
    }
  }
  
  return unculled;
}

float Camera::percentInView(std::shared_ptr<GameObject> object, WorldGrid *worldgrid)
{
  float result = 0.0f;

  OBB *targetBox = new OBB(object->position, object->dimensions);
  std::vector<glm::vec3> *corners = targetBox->getCorners();
  std::vector<glm::vec4> *planes = getViewFrustum(getProjection() * getView());
  std::vector<glm::vec3> cornersInView;
  for (auto cornerItr = corners->begin(); cornerItr != corners->end(); ++cornerItr) {
    bool insideFrustum = true;
    for (auto planeItr = planes->begin(); insideFrustum && planeItr != planes->end(); ++planeItr) {
      insideFrustum = pointOutsidePlane(*cornerItr, *planeItr);
    }
    if (insideFrustum) {
      cornersInView.push_back(*cornerItr);
    }
  }

  // don't bother doing VFC if player is outside of guard's view frustum
  if (cornersInView.size() == 0) {
    return result;
  }

  // is it worth doing VFC to get shorter list for this collision? try both ways and benchmark
#ifdef CULL_FIRST
  std::vector<std::shared_ptr<GameObject>> objInView = this->getUnculled(worldgrid);
#else
  std::vector<std::shared_ptr<GameObject>> objInView = worldgrid->list;
  std::vector<std::shared_ptr<GameObject>> walls = worldgrid->wallList;
  objInView.insert(objInView.begin(), walls.begin(), walls.end());  // whyyyyyyyyyyy
#endif
  float incr = 1.0f / cornersInView.size();
  for (auto objIter = objInView.begin(); objIter != objInView.end(); ++objIter) {
    for (auto cornerItr = cornersInView.begin(); cornerItr != cornersInView.end(); ++cornerItr) {
#ifdef DEBUG
      this->debug->addLine(this->eye, *cornerItr, glm::vec3(0.0f, 0.0f, 1.0f), false);
#endif
      OBB *hitBox = new OBB((*objIter)->position, (*objIter)->dimensions);
      float rayDist;
      if (rayOBBIntersect(&rayDist, (*objIter)->position, *cornerItr - this->eye, *hitBox)) {
        float lookDist = glm::distance(*cornerItr, this->eye);
        if (lookDist > rayDist) {
#ifdef DEBUG
          this->debug->addLine(this->eye, this->eye + glm::normalize(*cornerItr - this->eye) * rayDist, glm::vec3(1.0f, 0.0f, 0.0f), true);
#endif
          result += incr;
        }
      }
      else {
        result += incr;
      }
    }
  }

  return result;
}
