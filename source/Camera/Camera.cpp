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
  std::vector<glm::vec4> planes;
  glm::mat4 VP = this->getProjection() * this->getView();
  // the plane normals are not normalized, and point to the inside of the view frustum
  // plane equations obtained from http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
  // a plane is represented by a vec4 as <a, b, c, d>
  glm::vec4 left = glm::vec4(VP[0][3] + VP[0][0], VP[1][3] + VP[1][0], VP[2][3] + VP [2][0], VP[3][3] + VP[3][0]);
  glm::vec4 right = glm::vec4(VP[0][3] - VP[0][0], VP[1][3] - VP[1][0], VP[2][3] - VP [2][0], VP[3][3] - VP[3][0]);
  glm::vec4 bottom = glm::vec4(VP[0][3] + VP[0][1], VP[1][3] + VP[1][1], VP[2][3] + VP [2][1], VP[3][3] + VP[3][1]);
  glm::vec4 top = glm::vec4(VP[0][3] - VP[0][1], VP[1][3] - VP[1][1], VP[2][3] - VP [2][1], VP[3][3] - VP[3][1]);
  glm::vec4 nearPlane = glm::vec4(VP[0][3] + VP[0][2], VP[1][3] + VP[1][2], VP[2][3] + VP [2][2], VP[3][3] + VP[3][2]);
  glm::vec4 farPlane = glm::vec4(VP[0][3] - VP[0][2], VP[1][3] - VP[1][2], VP[2][3] - VP [2][2], VP[3][3] - VP[3][2]);

#ifdef DEBUG
  this->debug->addBox(left, right, bottom, top, nearPlane, farPlane, glm::vec3(0.99f, 0.85f, 0.55f), false, false);
#endif

  planes.push_back(left); // left
  planes.push_back(right); // right
  planes.push_back(bottom); // bottom
  planes.push_back(top); // top
  planes.push_back(nearPlane); // near
  planes.push_back(farPlane); // far

  std::vector<std::shared_ptr<GameObject>> allObjects = worldgrid->list;
  std::vector<std::shared_ptr<GameObject>> unculled;
  std::vector<std::shared_ptr<GameObject>> walls = worldgrid->wallList;
  allObjects.insert(allObjects.begin(), walls.begin(), walls.end());  // whyyyyyyyyyyy
  for (auto objIter = allObjects.begin(); objIter != allObjects.end(); ++objIter) {
    OBB *obb = new OBB((*objIter)->position, (*objIter)->dimensions);
#ifdef DEBUG
    //this->debug->addOBB(*obb, glm::vec3(0.0f, 0.0f, 1.0f), true);
#endif
    bool pass = true;
    for (auto planeIter = planes.begin(); pass && planeIter != planes.end(); ++planeIter) {
      pass = obbOutsidePlane(*obb, *planeIter);
    }
    if (pass) {
      unculled.push_back(*objIter);
    }
  }
  
  return unculled;
}
