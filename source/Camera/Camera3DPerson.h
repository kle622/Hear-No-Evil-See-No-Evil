#ifndef __CAMERA_3D_PERSON_H
#define __CAMERA_3D_PERSON_H

#include <memory>
#include <limits>
#include <cassert>
#include "Camera.h"
#include "../GameObject/Player.h"
#include "../GameObject/Wall.h"
#include "../GameObject/Guard.h"
#include "../Library/GLError.h"
#include <cmath>

#define UPPER_BOUND_DEFAULT 85.0f
#define LOWER_BOUND_DEFAULT -35.0f
//#define EPSILON .00001f

class Camera3DPerson: public Camera
{
  private:
    float theta;
    float phi;
    glm::vec3 getEye();
    glm::vec3 setZoom(glm::vec3 outDir);
    float castRayOnObjects(glm::vec3 rayStart, glm::vec3 rayEnd, std::vector<shared_ptr<GameObject>> objects);
  public:
    float zoom;
    float minZoom;
    float lowerBound; // bounds stored as degrees
    float upperBound;
    GameObject *focus;
    WorldGrid *world;
    Camera3DPerson(WorldGrid *world, GameObject *focus, float zoom, float fov, float aspect, float _near, float _far, DebugDraw *debug);
    void moveVert(float step);
    void moveHoriz(float step);

    // for performance reasons, call this only once per draw loop (once all positions are finalized)
    void update();
};

#endif
