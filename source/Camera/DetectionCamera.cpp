#include "DetectionCamera.h"
#define _USE_MATH_DEFINES

#define DEBUG
#define VERT_OFFSET 0.8f
#define CULL_FIRST

DetectionCamera::DetectionCamera(WorldGrid *world, GameObject *viewer, GameObject *target,
    float fov, float aspect, float _near, float _far, DebugDraw *debug) :
  Camera(target->position, viewer->position + glm::vec3(0.0f, VERT_OFFSET, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f), fov, aspect, _near, _far, debug)
{
  this->world = world;
  this->viewer = viewer;
  this->target = target;
}

void DetectionCamera::update()
{
  this->eye = this->viewer->position + glm::vec3(0.0f, VERT_OFFSET, 0.0f);
  this->lookat = this->viewer->position + this->viewer->direction + glm::vec3(0.0f, VERT_OFFSET, 0.0f);
}

float DetectionCamera::percentInView()
{
  float result = 0.0f;

  OBB *targetBox = new OBB(target->position, target->dimensions);
  std::vector<glm::vec3> *corners = targetBox->getCorners();
  std::vector<glm::vec4> *planes = getViewFrustum(getProjection() * getView());
  std::vector<glm::vec3> cornersInView;
#ifdef DEBUG
  // target box
  this->debug->addBox(this->target->position, this->target->dimensions, glm::vec3(1.0f, 1.0f, 0.0f), false, true);
  // viewer box
  this->debug->addBox(this->viewer->position, this->viewer->dimensions, glm::vec3(0.0f, 1.0f, 1.0f), false, true);
  // viewer frustum
  this->debug->addBox(planes->at(0), planes->at(1), planes->at(2), planes->at(3), planes->at(4), planes->at(5), glm::vec3(0.3f, 0.85f, 0.55f), false, false);
#endif
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
  std::vector<std::shared_ptr<GameObject>> objInView = this->getUnculled(world);
#else
  std::vector<std::shared_ptr<GameObject>> objInView = world->list;
  std::vector<std::shared_ptr<GameObject>> walls = world->wallList;
  objInView.insert(objInView.begin(), walls.begin(), walls.end());  // whyyyyyyyyyyy
#endif
  float incr = 1.0f / corners->size();
  for (auto cornerItr = cornersInView.begin(); cornerItr != cornersInView.end(); ++cornerItr) {
    float rayDist = 0;
    bool cornerInView = true;
    for (auto objIter = objInView.begin(); cornerInView && objIter != objInView.end(); ++objIter) {
      shared_ptr<GameObject> obj = *objIter;
      // don't check collisions against viewer and target
      if (this->viewer != obj.get() && this->target != obj.get()) {
        OBB *hitBox = new OBB(obj->position, obj->dimensions);
#ifdef DEBUG
        this->debug->addOBB(*hitBox, glm::vec3(1.0f, 1.0f, 1.0f), true, true);
#endif
        if (rayOBBIntersect(&rayDist, this->eye, *cornerItr - this->eye, *hitBox)) {
          float lookDist = glm::distance(*cornerItr, this->eye);
          if (rayDist < lookDist) {
            cornerInView = false;
#ifdef DEBUG
            this->debug->addLine(this->eye, this->eye + glm::normalize(*cornerItr - this->eye) * rayDist, glm::vec3(1.0f, 0.0f, 0.0f), true);
#endif
          }
        }
      }
    }
#ifdef DEBUG
    // ray from camera's eye to corner of box
    this->debug->addLine(this->eye, this->eye + glm::normalize(*cornerItr - this->eye) * glm::distance(*cornerItr, this->eye), glm::vec3(0.0f, 0.0f, 1.0f), false);
    if (cornerInView) {
      this->debug->addLine(this->eye, *cornerItr, glm::vec3(0.0f, 1.0f, 1.0f), true);
    }
#endif
    if (cornerInView) {
      result += incr;
    }
  }
  return result;
}
