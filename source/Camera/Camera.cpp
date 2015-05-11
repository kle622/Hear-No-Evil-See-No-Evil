#include "Camera.h"

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

// TODO implement to mimic getUnculled for one object
bool Camera::isCulled(shared_ptr<GameObject>)
{
  return false;
}

std::vector<std::shared_ptr<GameObject>> Camera::getUnculled(WorldGrid *worldgrid)
{
  // ----- View Frustum Culling -----
  std::vector<glm::vec4> planes;
  glm::mat4 VP = this->getProjection() * this->getView();
  // the plane normals are not normalized, and point to the inside of the view frustum
  // plane equations obtained from http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
  // a plane is represented by a vec4 as <a, b, c, d>
  /*glm::vec4 left = glm::vec4(VP[3][0] + VP[0][0], VP[3][1] + VP[0][1], VP[3][2] + VP [0][2], VP[3][3] + VP[0][3]);
  glm::vec4 right = glm::vec4(VP[3][0] - VP[0][0], VP[3][1] - VP[0][1], VP[3][2] - VP [0][2], VP[3][3] - VP[0][3]);
  glm::vec4 bottom = glm::vec4(VP[3][0] + VP[1][0], VP[3][1] + VP[1][1], VP[3][2] + VP [1][2], VP[3][3] + VP[1][3]);
  glm::vec4 top = glm::vec4(VP[3][0] - VP[1][0], VP[3][1] - VP[1][1], VP[3][2] - VP [1][2], VP[3][3] - VP[1][3]);
  glm::vec4 near = glm::vec4(VP[3][0] + VP[2][0], VP[3][1] + VP[2][1], VP[3][2] + VP [2][2], VP[3][3] + VP[2][3]);
  glm::vec4 far = glm::vec4(VP[3][0] - VP[2][0], VP[3][1] - VP[2][1], VP[3][2] - VP [2][2], VP[3][3] - VP[2][3]);*/
  
  // seems to be correct view frustum
  glm::vec4 left = glm::vec4(VP[0][3] + VP[0][0], VP[1][3] + VP[1][0], VP[2][3] + VP [2][0], VP[3][3] + VP[3][0]);
  glm::vec4 right = glm::vec4(VP[0][3] - VP[0][0], VP[1][3] - VP[1][0], VP[2][3] - VP [2][0], VP[3][3] - VP[3][0]);
  glm::vec4 bottom = glm::vec4(VP[0][3] + VP[0][1], VP[1][3] + VP[1][1], VP[2][3] + VP [2][1], VP[3][3] + VP[3][1]);
  glm::vec4 top = glm::vec4(VP[0][3] - VP[0][1], VP[1][3] - VP[1][1], VP[2][3] - VP [2][1], VP[3][3] - VP[3][1]);
  glm::vec4 near = glm::vec4(VP[0][3] + VP[0][2], VP[1][3] + VP[1][2], VP[2][3] + VP [2][2], VP[3][3] + VP[3][2]);
  glm::vec4 far = glm::vec4(VP[0][3] - VP[0][2], VP[1][3] - VP[1][2], VP[2][3] - VP [2][2], VP[3][3] - VP[3][2]);

#ifdef DEBUG
  this->debug->addBox(left, right, bottom, top, near, far, glm::vec3(0.99f, 0.85f, 0.55f), false);
#endif
  /*std::cout << "left: <" << left.x << ", " << left.y << ", " << left.z << ", " << left.w << ">" << std::endl;
  std::cout << "right: <" << right.x << ", " << right.y << ", " << right.z << ", " << right.w << ">" << std::endl;
  std::cout << "bottom: <" << bottom.x << ", " << bottom.y << ", " << bottom.z << ", " << bottom.w << ">" << std::endl;
  std::cout << "top: <" << top.x << ", " << top.y << ", " << top.z << ", " << top.w << ">" << std::endl;
  std::cout << "near: <" << near.x << ", " << near.y << ", " << near.z << ", " << near.w << ">" << std::endl;
  std::cout << "far: <" << far.x << ", " << far.y << ", " << far.z << ", " << far.w << ">" << std::endl;*/

  planes.push_back(left); // left
  planes.push_back(right); // right
  planes.push_back(bottom); // bottom
  planes.push_back(top); // top
  planes.push_back(near); // near
  planes.push_back(far); // far

  // TODO figure out how to draw view frustum wireframe

  std::vector<std::shared_ptr<GameObject>> allObjects = worldgrid->list;
  std::vector<std::shared_ptr<GameObject>> inVF;
  for (auto objIter = allObjects.begin(); objIter != allObjects.end(); ++objIter) {
    OBB *obb = new OBB((*objIter)->position, (*objIter)->dimensions);
#ifdef DEBUG
    //this->debug->addOBB(*obb, glm::vec3(0.0f, 0.0f, 1.0f), true);
#endif
    bool pass = true;
    for (auto planeIter = planes.begin(); pass && planeIter != planes.end(); ++planeIter) {
      pass = obbOutsidePlane(*obb, *planeIter);
      /*if (!obbOutsidePlane(*obb, *planeIter)) {
        pass = false;
      }*/
    }
    if (pass) {
      inVF.push_back(*objIter);
    }
  }

  // ----- Wall VFC -----
  std::vector<std::shared_ptr<GameObject>> walls = worldgrid->wallList;
  for (auto objIter = walls.begin(); objIter != walls.end(); ++objIter) {
    OBB *obb = new OBB((*objIter)->position, (*objIter)->dimensions);
#ifdef DEBUG
    //this->debug->addOBB(*obb, glm::vec3(0.0f, 0.0f, 1.0f), true);
#endif
    bool pass = true;
    for (auto planeIter = planes.begin(); pass && planeIter != planes.end(); ++planeIter) {
      pass = obbOutsidePlane(*obb, *planeIter);
      /*if (!obbOutsidePlane(*obb, *planeIter)) {
        pass = false;
      }*/
    }
    if (pass) {
      inVF.push_back(*objIter);
    }
  }

  // ----- Occlusion Culling -----
  std::vector<std::shared_ptr<GameObject>> unculled;
  for (auto objIter = inVF.begin(); objIter != inVF.end(); ++objIter) {
    unculled.push_back(*objIter); // TODO change this to actually be selective
  }

  return unculled;
}

// TODO implement (pseudocode in book)
// p. 755-757
// returns true if object is completely inside or intersects plane
// returns false if object is completely outside plane
//
// inside is defined as positive, outside is defined as negative
// plane is not assumed to be normalized
bool Camera::obbOutsidePlane(OBB obb, glm::vec4 plane)
{
  float nlength = glm::length(glm::vec3(plane.x, plane.y, plane.z));
  glm::vec3 c = obb.center;
  glm::vec3 n = glm::normalize(glm::vec3(plane.x, plane.y, plane.z));
  float d = plane.w / nlength;
  float e = obb.halfLengths[0] * glm::dot(n, obb.axes[0]) +
            obb.halfLengths[1] * glm::dot(n, obb.axes[1]) +
            obb.halfLengths[2] * glm::dot(n, obb.axes[2]);
  float s = glm::dot(c, n) + d;
  if (s - e > 0) {
    return true;
  }
  else if (s + e < 0) {
    return false;
  }
  else {
    return true;
  }
}

double clamp(double x, double min, double max) {
  return x < min ? min : (x > max ? max : x);
}
