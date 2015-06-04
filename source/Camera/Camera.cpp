#include "Camera.h"
#define _USE_MATH_DEFINES

#define DEBUG

Camera::Camera(glm::vec3 lookat, glm::vec3 eye, glm::vec3 up,
    float fov, float aspect, float _near, float _far, DebugDraw *debug)
{
  this->lookat = lookat;
  this->eye = eye;
  this->up = up;
  this->fov = fov;
  this->aspect = aspect;
  this->_near= _near;
  this->_far = _far;
  this->debug = debug;
}

glm::mat4 Camera::getView()
{
  return glm::lookAt(this->eye, this->lookat, this->up);
}

glm::mat4 Camera::getProjection()
{
  return glm::perspective(this->fov, this->aspect, this->_near, this->_far);
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
  if (NULL != debug) {
    debug->addBox(planes->at(0), planes->at(1), planes->at(2), planes->at(3), planes->at(4), planes->at(5), glm::vec3(0.99f, 0.85f, 0.55f), false, false);
  }
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

void Camera::moveHoriz(float step)
{
    theta += step * 80;

    lookat.x = eye.x + cos(phi * M_PI / 180) * cos(theta * M_PI / 180);
    lookat.y = eye.y + sin(phi * M_PI / 180);
    lookat.z = eye.z + cos(phi * M_PI / 180) * sin(-1.0 * theta * M_PI / 180);
}

void Camera::moveVert(float step)
{
    // note that ypos is measured from the top of the screen, so
    // an increase in ypos means moving the mouse down the y axis
    if ((phi < max_vert_angle && step < 0) || (phi > -1.0 * max_vert_angle && step > 0)) {
      phi -= step * 80;
    }

    lookat.x = eye.x + cos(phi * M_PI / 180) * cos(theta * M_PI / 180);
    lookat.y = eye.y + sin(phi * M_PI / 180);
    lookat.z = eye.z + cos(phi * M_PI / 180) * sin(-1.0 * theta * M_PI / 180);
}
