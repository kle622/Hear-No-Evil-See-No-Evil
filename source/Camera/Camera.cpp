#include "Camera.h"

Camera::Camera(Handles *handles, glm::vec3 lookat, glm::vec3 eye, glm::vec3 up,
    float fov, float aspect, float _near, float _far)
{
  this->handles = handles;
  this->lookat = lookat;
  this->eye = eye;
  this->up = up;
  this->fov = fov;
  this->aspect = aspect;
  this->_near = _near;
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

bool Camera::isCulled(shared_ptr<GameObject>)
{
  return false;
}

std::vector<std::shared_ptr<GameObject>> Camera::getUnculled(WorldGrid *worldgrid)
{
  std::vector<std::shared_ptr<GameObject>> unculled;
  std::vector<std::shared_ptr<GameObject>> inVF;
  std::vector<std::shared_ptr<GameObject>> allObjects = worldgrid->list;
  // ----- View Frustum Culling -----
  std::vector<glm::vec4> planes;
  glm::mat4 VP = this->getProjection() * this->getView();
  // the plane normals are not normalized, and point to the inside of the view frustum
  // plane equations obtained from http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
  planes.push_back(glm::vec4(VP[4][1] + VP[1][1], VP[4][2] + VP[1][2], VP[4][3] + VP [1][3], VP[4][4] + VP[1][4])); // left
  planes.push_back(glm::vec4(VP[4][1] - VP[1][1], VP[4][2] - VP[1][2], VP[4][3] - VP [1][3], VP[4][4] - VP[1][4])); // right
  planes.push_back(glm::vec4(VP[4][1] + VP[2][1], VP[4][2] + VP[2][2], VP[4][3] + VP [2][3], VP[4][4] + VP[2][4])); // bottom
  planes.push_back(glm::vec4(VP[4][1] - VP[2][1], VP[4][2] - VP[2][2], VP[4][3] - VP [2][3], VP[4][4] - VP[2][4])); // top
  planes.push_back(glm::vec4(VP[4][1] + VP[3][1], VP[4][2] + VP[3][2], VP[4][3] + VP [3][3], VP[4][4] + VP[3][4])); // near
  planes.push_back(glm::vec4(VP[4][1] - VP[3][1], VP[4][2] - VP[3][2], VP[4][3] - VP [3][3], VP[4][4] - VP[3][4])); // far

  for (auto objIter = allObjects.begin(); objIter != allObjects.end(); ++objIter) {
    OBB obb((*objIter)->position, (*objIter)->dimensions);
    for (auto planeIter = planes.begin(); planeIter != planes.end(); ++planeIter) {
      if (!obbInsidePlane(obb, *planeIter)) {
        objIter = allObjects.end();
      }
    }
    inVF.push_back(*objIter);
  }

  // ----- Occlusion Culling -----
  for (auto objIter = inVF.begin(); objIter != inVF.end(); ++objIter) {
    unculled.push_back(*objIter); // TODO change this to actually be selective
  }

  return unculled;
}

// TODO implement (pseudocode in book)
bool Camera::obbInsidePlane(OBB obb, glm::vec4 plane)
{
  return true;
}

double clamp(double x, double min, double max) {
  return x < min ? min : (x > max ? max : x);
}
