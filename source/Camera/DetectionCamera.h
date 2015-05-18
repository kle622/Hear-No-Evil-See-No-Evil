#ifndef __DETECTION_CAMERA_H_
#define __DETECTION_CAMERA_H_

#include "Camera.h"
#include "../GameObject/Guard.h"
#include "../GameObject/Player.h"

/*
 * Camera for visual detection. Has members for viewer and target (consider supporting multiple targets if necessary)
 */

class DetectionCamera: public Camera {
  public:
    //Properties
    WorldGrid *world;
    GameObject *viewer;
    GameObject *target;

    //Constructor
    DetectionCamera(WorldGrid *world, GameObject *viewer, GameObject *target, float fov, float aspect, float _near, float _far, DebugDraw *debug);

    float percentInView();
    // for performance reasons, call this only once per draw loop (once all positions are finalized)
    void update();
};

#endif
