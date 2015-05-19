#include "DetectionCamera.h"
#define _USE_MATH_DEFINES

#define DEBUG
#define VERT_OFFSET 0.8f
#define CULL_FIRST

DetectionCamera::DetectionCamera(float fov, float aspect, float _near, float _far, DebugDraw *debug) :
  Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f),
      glm::vec3(0.0f, 1.0f, 0.0f), fov, aspect, _near, _far, debug)
{
}

void DetectionCamera::update(GameObject *viewer)
{
  this->eye = viewer->position + glm::vec3(0.0f, VERT_OFFSET, 0.0f);
  this->lookat = viewer->position + viewer->direction + glm::vec3(0.0f, VERT_OFFSET, 0.0f);
}

float DetectionCamera::percentInView(WorldGrid *world, GameObject *viewer, GameObject *target)
{
  float result = 0.0f;

  update(viewer);

  OBB *targetBox = new OBB(target->position, target->dimensions);
  std::vector<glm::vec3> *corners = targetBox->getCorners();
  std::vector<glm::vec4> *planes = getViewFrustum(getProjection() * getView());
  corners->push_back(target->position);
  std::vector<glm::vec3> cornersInView;
#ifdef DEBUG
  if (NULL != debug) {
    // target box
    //debug->addBox(target->position, target->dimensions, glm::vec3(1.0f, 1.0f, 0.0f), false, true);
    // viewer box
    //debug->addBox(viewer->position, viewer->dimensions, glm::vec3(0.0f, 1.0f, 1.0f), false, true);
    // viewer frustum
    debug->addBox(planes->at(0), planes->at(1), planes->at(2), planes->at(3), planes->at(4), planes->at(5), glm::vec3(0.3f, 0.85f, 0.55f), false, false);
  }
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
      if (viewer != obj.get() && target != obj.get()) {
        OBB *hitBox = new OBB(obj->position, obj->dimensions);
        if (rayOBBIntersect(&rayDist, this->eye, glm::normalize(*cornerItr - this->eye), *hitBox)) {
          float lookDist = glm::distance(*cornerItr, this->eye);
          if (rayDist < lookDist) {
            cornerInView = false;
#ifdef DEBUG
            if (NULL != debug) {
              debug->addOBB(*hitBox, glm::vec3(1.0f, 1.0f, 1.0f), true, true);
              debug->addLine(this->eye, this->eye + glm::normalize(*cornerItr - this->eye) * rayDist, glm::vec3(1.0f, 0.0f, 0.0f), true);
            }
#endif
          }
        }
      }
    }
#ifdef DEBUG
    // ray from camera's eye to corner of box
    if (NULL != debug) {
      debug->addLine(this->eye, this->eye + glm::normalize(*cornerItr - this->eye) * glm::distance(*cornerItr, this->eye), glm::vec3(0.0f, 0.0f, 1.0f), false);
      if (cornerInView) {
        debug->addLine(this->eye, *cornerItr, glm::vec3(0.0f, 1.0f, 1.0f), true);
      }
    }
#endif
    if (cornerInView) {
      result += incr;
    }
  }
  return result;
}
